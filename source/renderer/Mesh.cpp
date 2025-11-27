#include "Mesh.hpp"
#include "GlobalConstantBufferManager.hpp"
#include "RenderContextImmediate.hpp"
#include "RenderMaterial.hpp"
#include "QuadIndexBuffer.hpp"
#include "MatrixStack.hpp"

using namespace mce;

Mesh::Mesh()
{
    m_indexCount = 0;
    m_vertexCount = 0;
    m_primitiveMode = PRIMITIVE_MODE_NONE;
    m_indexSize = 0;
    m_rawData = nullptr;
}

Mesh::Mesh(Mesh& other)
{
    _move(other);
}

Mesh::Mesh(const VertexFormat& vertexFormat, unsigned int vertexCount, unsigned int indexCount, uint8_t indexSize, PrimitiveMode primitiveMode, uint8_t *data, bool temporary)
    : m_vertexCount(vertexCount)
    , m_indexCount(indexCount)
    , m_primitiveMode(primitiveMode)
    , m_vertexFormat(vertexFormat)
    , m_indexSize(indexSize)
{
    if (temporary)
    {
        m_rawData = data;
    }
    else
    {
        m_rawData = nullptr;
        if (!loadRawData(RenderContextImmediate::get(), data))
        {
            reset();
        }
    }
}

Mesh::~Mesh()
{
    reset();
}

void Mesh::_move(Mesh& other)
{
    this->m_vertexBuffer = std::move(other.m_vertexBuffer);
    this->m_indexBuffer = std::move(other.m_indexBuffer);
    this->m_vertexCount = other.m_vertexCount;
    this->m_vertexFormat = other.m_vertexFormat;
    this->m_indexCount = other.m_indexCount;
    this->m_indexSize = other.m_indexSize;
    this->m_primitiveMode = other.m_primitiveMode;
    this->m_rawData = other.m_rawData;

    other.m_vertexFormat = VertexFormat::EMPTY;
    other.m_indexCount = 0;
    other.m_vertexCount = 0;
}

void Mesh::_refreshMatrices()
{
#if FEATURE_GFX_SHADERS
    GlobalConstantBufferManager& bufferManager = GlobalConstantBufferManager::getInstance();
    bufferManager.refreshWorldConstants();
#elseif ENH_GFX_MATRIX_STACK
    // copied from WorldConstants::refreshWorldConstants()

    // @TODO: keep MatrixStack states in RC, make MatrixStack ID enum, make loadMatrixStack(stackIdEnum, stackPtr)

    if (MatrixStack::Projection.isDirty())
    {
        const Matrix& matrix = MatrixStack::Projection.top();

        // @TODO: abstract
        glMatrixMode(GL_PROJECTION);
        xglLoadTransposeMatrixf((float*)&matrix._m);
        glMatrixMode(GL_MODELVIEW);

        MatrixStack::Projection.makeClean();
    }

    if (MatrixStack::World.isDirty() || MatrixStack::View.isDirty())
    {
        const Matrix& worldMatrix = MatrixStack::World.top();
        const Matrix& viewMatrix = MatrixStack::View.top();
        Matrix modelViewMatrix = worldMatrix * viewMatrix;

        // @TODO: abstract
        glMatrixMode(GL_MODELVIEW);
        xglLoadTransposeMatrixf((float*)&modelViewMatrix._m);

        MatrixStack::World.makeClean();
        MatrixStack::View.makeClean();
    }
#endif
}

void Mesh::reset()
{
    m_vertexBuffer.releaseBuffer();
    m_indexBuffer.releaseBuffer();
    m_vertexCount = 0;
    m_indexCount = 0;
    m_primitiveMode = PRIMITIVE_MODE_NONE;
    m_vertexFormat = VertexFormat::EMPTY;
    m_indexSize = 0;
    m_rawData = nullptr;
}

bool Mesh::loadRawData(RenderContext& context, uint8_t *data)
{
    if (!data)
        return false;
    if (m_vertexCount <= 2)
        return false;
    if (m_primitiveMode == PRIMITIVE_MODE_NONE)
        return false;
    if (!m_vertexFormat)
        return false;
    
    unsigned int vertexSize = m_vertexFormat.getVertexSize();
    m_vertexBuffer.createVertexBuffer(context, vertexSize, data, m_vertexCount);

    if (m_indexCount == 0)
        return true;

    unsigned int index = m_vertexCount * vertexSize;
    m_indexBuffer.createIndexBuffer(context, m_indexSize, &data[index], m_indexCount);

    return true;
}

void Mesh::render(const MaterialPtr& materialPtr, unsigned int startOffset, unsigned int count)
{
    RenderContext& context = RenderContextImmediate::get();

    if (!isValid())
        return;

    unsigned int vertexCount = (count > 0) ? count : m_vertexCount;

    if (isTemporary())
    {
        ImmediateBuffer& immediateBuffer = context.m_immediateBuffer; 

        if (!immediateBuffer.isValid())
        {
            // Create 1MB shared vertex buffer in VRAM
            immediateBuffer.createDynamicBuffer(context, 1, nullptr, 0x100000, BUFFER_TYPE_VERTEX);
        }

        unsigned int vertexSize = m_vertexFormat.getVertexSize();
        void* vertexData = m_rawData;
        immediateBuffer.updateBuffer(context, vertexSize, vertexData, vertexCount);
    }
    else
    {
        m_vertexBuffer.bindBuffer(context);
    }

    GlobalConstantBufferManager& bufferManager = GlobalConstantBufferManager::getInstance();
    bufferManager.refreshWorldConstants();

    if (materialPtr)
    {
        materialPtr->useWith(context, m_vertexFormat, m_rawData);
#ifdef FEATURE_GFX_SHADERS
        materialPtr->m_pShader->validateVertexFormat(m_vertexFormat);
#endif
    }

#ifndef FEATURE_GFX_SHADERS
    context.setDrawState(m_vertexFormat);
#endif
    if (m_primitiveMode == PRIMITIVE_MODE_QUAD_LIST)
    {
        uint8_t indexSize = m_indexSize;
        Buffer& quadIndexBuffer = QuadIndexBuffer::get(context, m_vertexCount, indexSize);
        quadIndexBuffer.bindBuffer(context);

        unsigned int indexCountToDraw = (count > 0) ? count : (m_vertexCount / 4) * 6; // 6 vertecies per quad
        context.drawIndexed(m_primitiveMode, indexCountToDraw, startOffset, indexSize);
    }
    else if (m_indexCount > 0)
    {
        m_indexBuffer.bindBuffer(context);
        
        unsigned int indexCountToDraw = (count > 0) ? count : m_indexCount;
        context.drawIndexed(m_primitiveMode, indexCountToDraw, startOffset, m_indexSize);
    }
    else
    {
        context.draw(m_primitiveMode, startOffset, vertexCount);
    }
#ifndef FEATURE_GFX_SHADERS
    context.clearDrawState(m_vertexFormat);
#endif
}

bool Mesh::isValid() const
{
    return ((m_vertexBuffer.isValid() && m_vertexCount > 0) || isTemporary()) && m_vertexFormat;
}

bool Mesh::isTemporary() const
{
    return m_rawData != nullptr;
}

Mesh& Mesh::operator=(Mesh& other)
{
    _move(other);
    return *this;
}

void Mesh::clearGlobalBuffers()
{
    // Empty on OGL
}