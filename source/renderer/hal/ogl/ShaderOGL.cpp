#include <typeinfo>
#include <stdexcept>

#include "ShaderOGL.hpp"

#include "renderer/GlobalConstantBufferManager.hpp"
#include "renderer/ConstantBufferMetaDataManager.hpp"
#include "renderer/RenderContextImmediate.hpp"

using namespace mce;

static ShaderOGL::VertexFieldFormat vertexFieldFormats[] = {
    { GL_FLOAT,          3, GL_FALSE }, // VERTEX_FIELD_POSITION
    { GL_UNSIGNED_BYTE,  4, GL_TRUE  }, // VERTEX_FIELD_COLOR
    { GL_BYTE,           4, GL_FALSE }, // VERTEX_FIELD_NORMAL
    { GL_UNSIGNED_SHORT, 2, GL_TRUE  }, // VERTEX_FIELD_UV0
    { GL_UNSIGNED_SHORT, 2, GL_TRUE  }  // VERTEX_FIELD_UV1
};

ShaderOGL::ShaderOGL(ShaderProgram& vertex, ShaderProgram& fragment, ShaderProgram& geometry)
    : ShaderBase(vertex, fragment, geometry)
{
    m_program = GL_NONE;

    createAndAttachPrograms();
    linkShader();
    reflectShader();
}

ShaderOGL::~ShaderOGL()
{
    deleteShader();
}

ShaderPrimitiveTypes ShaderOGL::shaderPrimitiveTypeFromOGLUniformType(GLenum uniformType)
{
    switch (uniformType)
    {
        case GL_INT:        return SHADER_PRIMITIVE_INT1;
        case GL_INT_VEC2:   return SHADER_PRIMITIVE_INT2;
        case GL_INT_VEC3:   return SHADER_PRIMITIVE_INT3;
        case GL_INT_VEC4:   return SHADER_PRIMITIVE_INT4;
        case GL_FLOAT:      return SHADER_PRIMITIVE_FLOAT1;
        case GL_FLOAT_VEC2: return SHADER_PRIMITIVE_FLOAT2;
        case GL_FLOAT_VEC3: return SHADER_PRIMITIVE_FLOAT3;
        case GL_FLOAT_VEC4: return SHADER_PRIMITIVE_FLOAT4;
        case GL_FLOAT_MAT2: return SHADER_PRIMITIVE_MATRIX2x2;
        case GL_FLOAT_MAT3: return SHADER_PRIMITIVE_MATRIX3x3;
        case GL_FLOAT_MAT4: return SHADER_PRIMITIVE_MATRIX4x4;
        case GL_SAMPLER_2D: return SHADER_PRIMITIVE_SAMPLER1D;
        default:
            LOG_E("Unknown type: %d", uniformType);
            throw std::bad_cast();
    }
}

void ShaderOGL::deleteShader()
{
    xglDeleteProgram(m_program);
    m_program = GL_NONE;
}

void ShaderOGL::finalizeShaderUniforms()
{
    for (int i = 0; i < m_uniformList.size(); i++)
    {
        ShaderUniformOGL& uniform = m_uniformList[i];
        uniform.m_shaderConstant = uniform.m_constantBufferContainer->getUnspecializedShaderConstant(uniform.m_name);
    }
}

void ShaderOGL::freeCompilerResources()
{
    xglReleaseShaderCompiler();
    glGetError();
}

void ShaderOGL::resetLastProgram()
{
    RenderContextImmediate::get().m_activeShaderProgram = GL_NONE;
}

void ShaderOGL::createAndAttachPrograms()
{
    m_program = xglCreateProgram();

    xglAttachShader(m_program, m_vertexShader.m_shaderName);
    xglAttachShader(m_program, m_fragmentShader.m_shaderName);
    if (m_geometryShader.isValid())
        xglAttachShader(m_program, m_geometryShader.m_shaderName);
    
    ErrorHandler::checkForErrors();
}

void ShaderOGL::linkShader()
{
    xglLinkProgram(m_program);

    ErrorHandler::checkForErrors();

    GLint linkStatus;
    xglGetProgramiv(m_program, GL_LINK_STATUS, &linkStatus);

    if (linkStatus == GL_TRUE)
        return;

    if (m_geometryShader.isValid())
    {
        LOG_E("Failed to link %s to %s and %s", m_vertexShader.m_shaderPath, m_fragmentShader.m_shaderPath, m_geometryShader.m_shaderPath);
        throw std::bad_cast();
    }
    else
    {
        LOG_E("Failed to link %s to %s", m_vertexShader.m_shaderPath, m_fragmentShader.m_shaderPath);
        throw std::bad_cast();
    }

    GLint logLength = 0;
    xglGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLength);

    if (logLength > 1)
    {
        int charsWritten = 0;
        char* infoLog;
        xglGetProgramInfoLog(m_program, logLength, &charsWritten, infoLog);

        LOG_E("Compiler error:\n%s", infoLog);
        throw std::bad_cast();
    }

    xglDeleteProgram(m_program);
    m_program = 0;
}

void ShaderOGL::bindVertexPointers(const VertexFormat& vertexFormat, const void* vertexData)
{
    RenderDevice& device = RenderDevice::getInstance();
    const RenderDeviceBase::AttributeList& attrList = device.getAttributeList(m_attributeListIndex);

    for (int i = 0; i < attrList.size(); i++)
    {
        const Attribute& attr = attrList[i];
        
        VertexField vertexField = attr.getVertexField();
        if (!vertexFormat.hasField(vertexField))
            continue;

        GLuint location = attr.getLocation();
        const VertexFieldFormat& format = vertexFieldFormats[vertexField];
        xglVertexAttribPointer(
            location,
            format.components,
            format.componentsType,
            format.normalized,
            vertexFormat.getVertexSize(),
            vertexFormat.getFieldOffset(vertexField, vertexData)
        );

        ErrorHandler::checkForErrors();
    }
}

void ShaderOGL::bindShader(RenderContext& context, const VertexFormat& format, const void *dataBasePtr, unsigned int shaderStageBits)
{
    bool shaderChanged = context.m_activeShaderProgram != m_program;

    if (shaderChanged)
    {
        xglUseProgram(m_program);
        context.m_activeShaderProgram = m_program;
    }

    bindVertexPointers(format, dataBasePtr);

    for (int i = 0; i < m_textureList.size(); i++)
    {
        const ShaderResourceOGL& resource = m_textureList[i];
        RenderContextOGL::ActiveTextureUnit& activeTextureUnit = context.getActiveTextureUnit(i);
        if ((shaderChanged && resource.m_bValid) || activeTextureUnit.m_bIsShaderUniformDirty)
        {
            xglUniform1i(resource.m_location, activeTextureUnit.m_textureUnit);
            activeTextureUnit.m_bIsShaderUniformDirty = false;
        }
    }

    for (int i = 0; i < m_uniformList.size(); i++)
    {
        ShaderUniformOGL& shaderUniform = m_uniformList[i];
        shaderUniform.bind(shaderChanged);
    }
}

void ShaderOGL::reflectShaderUniforms()
{
    GlobalConstantBufferManager& bufferManager = GlobalConstantBufferManager::getInstance();

    GLint uniformCount;
    xglGetProgramiv(m_program, GL_ACTIVE_UNIFORMS, &uniformCount);

    GLsizei length;
    GLenum type;
    GLint size;
    char name[1024];
    for (int i = 0; i < uniformCount; i++)
    {
        xglGetActiveUniform(m_program, i, 1024, &length, &size, &type, name);
        GLint location = xglGetUniformLocation(m_program, name);

        if (location < 0)
            continue;

        ShaderPrimitiveTypes shaderPrimitiveType = shaderPrimitiveTypeFromOGLUniformType(type);

        if (shaderPrimitiveType == SHADER_PRIMITIVE_SAMPLER1D &&
            strlen(name) > 8 && strstr(name, "TEXTURE_") != NULL)
        {
            ShaderResourceOGL texture(name, location, size, shaderPrimitiveType);
            texture.m_bValid = true;
            m_textureList.push_back(texture);
            if (m_textureList.size() > 8)
            {
                LOG_E("You've exeeded the gl spec for minimum number of texture units: %d", 8);
                throw std::bad_cast();
            }
        }
        else
        {
            ShaderUniformOGL uniform(name, location, size, shaderPrimitiveType);

            const UniformMetaData& uniformMetadata = ConstantBufferMetaDataManager::getInstance().findUniformMetaData(name);
            const std::string& bufferName = uniformMetadata.m_constantBufferMetaDataParent->getConstantBufferName();
            ConstantBufferContainer* pBufferContainer = bufferManager.findConstantBufferContainer(bufferName);
            pBufferContainer->registerReflectedShaderParameter(uniformMetadata);
            uniform.m_constantBufferContainer = pBufferContainer;

            m_uniformList.push_back(uniform);
        }
    }
}

void ShaderOGL::reflectShaderAttributes()
{
    RenderDeviceBase::AttributeList attrList;

    GLint attrCount;
    xglGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTES, &attrCount);

    if (attrCount)
    {
        char name[1024];
        GLsizei nameLen;
        GLint size;
        GLenum type;

        for (GLint i = 0; i < attrCount; i++)
        {
            xglGetActiveAttrib(m_program, i, sizeof(name), &nameLen, &size, &type, name);
            GLint location = xglGetAttribLocation(this->m_program, name);
            if (location < 0)
                continue;

            std::string attrName(name);
            VertexField vertexField = getAttributeForName(attrName, 0);

            Attribute attr(location, size, vertexField);
            attrList.push_back(attr);

            xglEnableVertexAttribArray(location);
            xglVertexAttribPointer(0, 1, GL_UNSIGNED_BYTE, 0, 1, this);
        }
    }

    m_attributeListIndex = RenderDevice::getInstance().registerOrGetAttributeListIndex(attrList);
}

void ShaderOGL::reflectShader()
{
    reflectShaderUniforms();
    reflectShaderAttributes();
}
