#include <typeinfo>
#include <stdexcept>
#include <sstream>

#include "ShaderD3D9.hpp"

#include "API_D3D9Compiler.hpp"

#include "common/Util.hpp"
#include "client/app/Minecraft.hpp"
#include "renderer/VertexFieldFormat.hpp"
#include "renderer/GlobalConstantBufferManager.hpp"
#include "renderer/ConstantBufferMetaDataManager.hpp"
#include "renderer/RenderContextImmediate.hpp"
#include "renderer/hal/d3d9/helpers/ErrorHandlerD3D9.hpp"

using namespace mce;

ShaderD3D9::ShaderD3D9(ShaderProgram& vertex, ShaderProgram& fragment, ShaderProgram& geometry)
    : ShaderBase(vertex, fragment, geometry)
{
    m_bCompiledShaders = false;
}

ShaderD3D9::~ShaderD3D9()
{
}

D3DDECLTYPE d3dDeclTypeFromVertexFieldType(VertexFieldType vertexFieldType)
{
    switch (vertexFieldType)
    {
    case VERTEX_FIELD_TYPE_UINT8_4:    return D3DDECLTYPE_UBYTE4;
    case VERTEX_FIELD_TYPE_UINT8_4_N:  return D3DDECLTYPE_UBYTE4N;
    case VERTEX_FIELD_TYPE_SINT16_2:   return D3DDECLTYPE_SHORT2;
    case VERTEX_FIELD_TYPE_SINT16_2_N: return D3DDECLTYPE_SHORT2N;
    case VERTEX_FIELD_TYPE_SINT16_4:   return D3DDECLTYPE_SHORT4;
    case VERTEX_FIELD_TYPE_SINT16_4_N: return D3DDECLTYPE_SHORT4N;
    case VERTEX_FIELD_TYPE_UINT16_2_N: return D3DDECLTYPE_USHORT2N;
    case VERTEX_FIELD_TYPE_UINT16_4_N: return D3DDECLTYPE_USHORT4N;
    case VERTEX_FIELD_TYPE_FLOAT32:    return D3DDECLTYPE_FLOAT1;
    case VERTEX_FIELD_TYPE_FLOAT32_2:  return D3DDECLTYPE_FLOAT2;
    case VERTEX_FIELD_TYPE_FLOAT32_3:  return D3DDECLTYPE_FLOAT3;
    case VERTEX_FIELD_TYPE_FLOAT32_4:  return D3DDECLTYPE_FLOAT4;

    default:
        LOG_W("Unknown vertexFieldType: %d", vertexFieldType);
        assert(false);
        return D3DDECLTYPE_UNUSED;
    }
}

D3DDECLUSAGE d3dDeclUsageFromVertexField(VertexField vertexField, int& outUsageIndex)
{
    outUsageIndex = 0;
    switch (vertexField)
    {
    case VERTEX_FIELD_POSITION: return D3DDECLUSAGE_POSITION;
    case VERTEX_FIELD_COLOR:    return D3DDECLUSAGE_COLOR;
    case VERTEX_FIELD_NORMAL:   return D3DDECLUSAGE_NORMAL;
    case VERTEX_FIELD_UV0:      outUsageIndex = 0; return D3DDECLUSAGE_TEXCOORD;
    case VERTEX_FIELD_UV1:      outUsageIndex = 1; return D3DDECLUSAGE_TEXCOORD;
    default:
        LOG_W("Unknown vertexField: %d", vertexField);
        assert(false);
        return D3DDECLUSAGE_TEXCOORD; // worst case we fuck up the UVs
    }
}

// Helper to map D3DDECLUSAGE to string for ShaderBase::getAttributeForName
const char* semanticNameFromD3DDeclUsage(UINT usage)
{
    switch (usage)
    {
    case D3DDECLUSAGE_POSITION:     return "POSITION";
    case D3DDECLUSAGE_BLENDWEIGHT:  return "BLENDWEIGHT";
    case D3DDECLUSAGE_BLENDINDICES: return "BLENDINDICES";
    case D3DDECLUSAGE_NORMAL:       return "NORMAL";
    case D3DDECLUSAGE_PSIZE:        return "PSIZE";
    case D3DDECLUSAGE_TEXCOORD:     return "TEXCOORD";
    case D3DDECLUSAGE_TANGENT:      return "TANGENT";
    case D3DDECLUSAGE_BINORMAL:     return "BINORMAL";
    case D3DDECLUSAGE_TESSFACTOR:   return "TESSFACTOR";
    //case D3DDECLUSAGE_POSITIONT:    return "POSITIONT";
    case D3DDECLUSAGE_COLOR:        return "COLOR";
    case D3DDECLUSAGE_FOG:          return "FOG";
    case D3DDECLUSAGE_DEPTH:        return "DEPTH";
    case D3DDECLUSAGE_SAMPLE:       return "SAMPLE";
    default:                        return "UNKNOWN";
    }
}

ShaderResourceD3D9* ShaderD3D9::_getShaderResource(const std::string& name)
{
    for (std::vector<ShaderResourceD3D9>::iterator it = m_resourceList.begin(); it != m_resourceList.end(); it++)
    {
        if (it->m_pShaderConstant->getName() == name)
        {
            return &*it;
        }
    }

    return nullptr;
}

void ShaderD3D9::bindVertexPointers(RenderContext& context, const VertexFormat& vertexFormat)
{
    // D3D9 stores Vertex Declarations instead of Input Layouts.
    // We construct a cache ID based on the vertex format. 

    RenderContextD3D9::VertexDeclID declId(vertexFormat, m_attributeListIndex);
    if (declId.vertexFormat == context.m_lastVertexFormat
        && declId.attributeListIndex == context.m_lastAttributeListIndex)
    {
        return;
    }

    ComInterface<IDirect3DVertexDeclaration9> vertexDecl;
    {
        RenderContextD3D9::VertexDeclCache::iterator vertexDeclCacheItr = context.m_vertexDeclCache.find(declId);
        if (vertexDeclCacheItr == context.m_vertexDeclCache.end())
        {
            vertexDecl = createVertexDeclaration(vertexFormat);
            context.m_vertexDeclCache[declId] = vertexDecl;
        }
        else
        {
            vertexDecl = vertexDeclCacheItr->second;
        }
    }

    {
        D3DDevice d3dDevice = context.getD3DDevice();
        d3dDevice->SetVertexDeclaration(**vertexDecl);
    }

    context.m_lastVertexFormat = declId.vertexFormat;
    context.m_lastAttributeListIndex = declId.attributeListIndex;
}

void ShaderD3D9::bindShader(RenderContext& context, const VertexFormat& format, const void* dataBasePtr, unsigned int shaderStageBits)
{
    if (!m_bCompiledShaders)
        compileAndLinkShader();

    bindVertexPointers(context, format);

    bool changedShaderProgram[SHADER_TYPES_COUNT] = { false };

    {
        D3DDevice d3dDevice = context.getD3DDevice();

        // Bind Vertex Shader
        if (context.m_lastShaderPrograms[SHADER_TYPE_VERTEX] != &m_vertexShader)
        {
            d3dDevice->SetVertexShader(**m_vertexShader.m_vertexShader);
            context.m_lastShaderPrograms[SHADER_TYPE_VERTEX] = &m_vertexShader;
            changedShaderProgram[SHADER_TYPE_VERTEX] = true;
        }

        // Bind Pixel Shader
        if (context.m_lastShaderPrograms[SHADER_TYPE_FRAGMENT] != &m_fragmentShader)
        {
            d3dDevice->SetPixelShader(**m_fragmentShader.m_pixelShader);
            context.m_lastShaderPrograms[SHADER_TYPE_FRAGMENT] = &m_fragmentShader;
            changedShaderProgram[SHADER_TYPE_FRAGMENT] = true;
        }
    }

    // Update Constant Buffers
    for (size_t i = 0; i < m_resourceList.size(); i++)
    {
        for (unsigned int t = 0; t < SHADER_TYPES_COUNT; t++)
        {
            ShaderResourceD3D9& resource = m_resourceList[i];
            if (((1 << t) & resource.m_shaderStagesBits) != 0)
            {
#if MCE_GFX_D3D9_SHADER_CONSTANT_BUFFERS
                resource.m_pConstantBufferContainer->bindBuffer(context, resource.m_shaderBindPoints[t], resource.m_shaderStagesBits);
#else
                resource.bind(context, (ShaderType)t, changedShaderProgram[t]);
#endif
            }
        }
    }
}

void ShaderD3D9::compileAndLinkShader()
{
    if (m_bCompiledShaders)
        return;

    m_vertexShader.compileShaderProgram();
    m_fragmentShader.compileShaderProgram();

    reflectShader(m_vertexShader,   SHADER_TYPE_VERTEX);
    reflectShader(m_fragmentShader, SHADER_TYPE_FRAGMENT);

    m_bCompiledShaders = true;
}

std::string _getConstantName(const char* name)
{
    // Finds the first occurance of a '+' and returns the rest of the string
    const char* it = name;
    while (*it != '\0')
    {
        if (*it == '+')
            return ++it;
        it++;
    }

    return name;
}

void ShaderD3D9::reflectShaderResources(ComInterface<ID3DXConstantTable> constantTable, ShaderType shaderType)
{
    GlobalConstantBufferManager& bufferManager = GlobalConstantBufferManager::getInstance();
    unsigned int shaderStagesBits = 1 << shaderType;

    D3DXCONSTANTTABLE_DESC tableDesc;
    constantTable->GetDesc(&tableDesc);

    // In D3D9, "Constant Buffers" as such don't exist in the hardware.
    // High-level semantics often treat a `struct` or a specific naming convention as a buffer.
    // We iterate the constants to find matching Container names.

    for (UINT i = 0; i < tableDesc.Constants; i++)
    {
        D3DXHANDLE hConstant = constantTable->GetConstant(NULL, i);
        D3DXCONSTANT_DESC constDesc;
        UINT count = 1;
        constantTable->GetConstantDesc(hConstant, &constDesc, &count);

        if (constDesc.RegisterSet == D3DXRS_SAMPLER)
            continue;

        std::string constName = _getConstantName(constDesc.Name);

        // We assume the shader uses structs to represent CBuffers, e.g., "struct GlobalConstants { ... } GlobalConstants;"
        // The name "GlobalConstants" will appear here with Class=D3DXPC_STRUCT.

        ShaderResourceD3D9* pShaderResource = _getShaderResource(constName);

        if (pShaderResource)
        {
            pShaderResource->m_shaderBindPoints[shaderType] = constDesc.RegisterIndex;
            pShaderResource->m_shaderStagesBits |= shaderStagesBits;
        }
        else
        {
            // Attempt to find a registered container with this name
            const UniformMetaData* uniformMetadata = ConstantBufferMetaDataManager::getInstance().tryFindUniformMetaData(constName);
            if (uniformMetadata)
            {
                const std::string& bufferName = uniformMetadata->m_constantBufferMetaDataParent->getConstantBufferName();
                ConstantBufferContainer* pConstantBufferContainer = bufferManager.findConstantBufferContainer(bufferName);

                if (pConstantBufferContainer)
                {
                    // We found a match. Register it.
                    ShaderResourceD3D9 shaderResource(constName);
                    shaderResource.m_pConstantBufferContainer = pConstantBufferContainer;
                    shaderResource.m_pShaderConstant = pConstantBufferContainer->getUnspecializedShaderConstant(constName);
                    shaderResource.m_shaderBindPoints[shaderType] = constDesc.RegisterIndex;
                    shaderResource.m_shaderStagesBits |= shaderStagesBits;

                    //pConstantBufferContainer->registerReflectedShaderParameter(uniformMetadata);

                    m_resourceList.push_back(shaderResource);
                }
            }
        }
    }
}

void ShaderD3D9::reflectShaderAttributes(const std::string& shaderBytecode)
{
    UINT count = 0;
    const DWORD* pFunction = (const DWORD*)shaderBytecode.c_str();

    // Get the number of semantics
    HRESULT hr = D3DXGetShaderInputSemantics(pFunction, NULL, &count);
    ErrorHandlerD3D9::checkForErrors(hr);

    if (count == 0)
        return;

    std::vector<D3DXSEMANTIC> semantics(count);

    // Retrieve the semantics
    hr = D3DXGetShaderInputSemantics(pFunction, &semantics[0], &count);
    ErrorHandlerD3D9::checkForErrors(hr);

    RenderDeviceBase::AttributeList attrList;

    for (UINT i = 0; i < count; i++)
    {
        const D3DXSEMANTIC& sem = semantics[i];

        std::string semanticName = semanticNameFromD3DDeclUsage(sem.Usage);

        // getAttributeForName maps (Name, Index) -> VertexField (e.g., "TEXCOORD", 1 -> VERTEX_FIELD_UV1)
        VertexField vertexField = getAttributeForName(semanticName, sem.UsageIndex);

        if (vertexField != VERTEX_FIELD_INVALID)
        {
            // We use 'i' as the location, representing the index in the declaration order
            attrList.push_back(Attribute(i, sem.UsageIndex, vertexField));
        }
    }

    m_attributeListIndex = RenderDevice::getInstance().registerOrGetAttributeListIndex(attrList);
}

void ShaderD3D9::reflectShader(const ShaderProgramD3D9& shaderProgram, ShaderType shaderType)
{
    if (!shaderProgram.isValid())
    {
        LOG_E("Tried to reflect invalid ShaderProgram. Are you missing a shader file?");
        throw std::bad_cast();
    }

    const std::string& shaderBytecode = shaderProgram.m_shaderBytecode;
    assert(!shaderBytecode.empty());

    ComInterface<ID3DXConstantTable> reflector;
    {
        HRESULT hr = D3DXGetShaderConstantTable((const DWORD*)shaderBytecode.c_str(), *reflector);
        ErrorHandlerD3D9::checkForErrors(hr);
    }

    if (shaderType == SHADER_TYPE_VERTEX)
        reflectShaderAttributes(shaderBytecode);
    reflectShaderResources(reflector, shaderType);
}

/*struct _VertexElementSorter
{
    bool operator()(const D3DVERTEXELEMENT9& a, const D3DVERTEXELEMENT9& b) const
    {
        if (a.Stream != b.Stream)
            return a.Stream < b.Stream;
        return a.Offset < b.Offset;
    }
};*/

ComInterface<IDirect3DVertexDeclaration9> ShaderD3D9::createVertexDeclaration(const VertexFormat& vertexFormat)
{
    RenderDevice& renderDevice = RenderDevice::getInstance();
    RenderContext& renderContext = renderDevice.getRenderContext();

    // We map the abstract AttributeList logic to D3DVertexElements
    RenderDeviceBase::AttributeList& attrList = renderDevice.m_registeredAttributeLists[m_attributeListIndex];

    std::vector<D3DVERTEXELEMENT9> elements;

    for (size_t i = 0; i < attrList.size(); i++)
    {
        Attribute& attr = attrList[i];
        VertexField vertexField = attr.getVertexField();

        // Calculate offset if not explicit
        WORD offset = (WORD)((intptr_t)vertexFormat.getFieldOffset(vertexField));

        int usageIndex = 0;
        D3DDECLUSAGE usage = d3dDeclUsageFromVertexField(vertexField, usageIndex);
        D3DDECLTYPE type = d3dDeclTypeFromVertexFieldType(vertexFieldTypes[vertexField]);

        D3DVERTEXELEMENT9 elem;
        elem.Stream = 0; // Always stream 0 for this architecture
        elem.Offset = offset;
        elem.Type = type;
        elem.Method = D3DDECLMETHOD_DEFAULT;
        elem.Usage = (BYTE)usage;
        elem.UsageIndex = (BYTE)usageIndex;

        elements.push_back(elem);
    }

    // Nothing in the docs says we need to order this in any particular way
    //std::sort(elements.begin(), elements.end(), _VertexElementSorter());

    // D3DVERTEXELEMENT9 array must be terminated with D3DDECL_END()
    D3DVERTEXELEMENT9 endElem = D3DDECL_END();
    elements.push_back(endElem);

    IDirect3DVertexDeclaration9* pDecl = nullptr;
    D3DDevice d3dDevice = renderContext.getD3DDevice();

    HRESULT hr = d3dDevice->CreateVertexDeclaration(&elements[0], &pDecl);
    ErrorHandlerD3D9::checkForErrors(hr);

    // @TODO: probably calling an extra AddRef here, does this mean D3D9 never releases the memory?
    return ComInterface<IDirect3DVertexDeclaration9>(pDecl);
}

void ShaderD3D9::SpliceShaderPath(std::string& shaderName)
{
    ShaderBase::SpliceShaderPath(shaderName, "/hlsl");
}

void ShaderD3D9::SpliceShaderExtension(std::string& shaderName)
{
    ShaderBase::SpliceShaderExtension(shaderName, ".hlsl");
}

void ShaderD3D9::BuildHeader(std::ostringstream& stream)
{
    stream << "#define _DIRECT3D9\n";
#ifdef _XBOX
    stream << "#define _XBOX\n";
#endif

    // hack to fix dumb D3D9 bullshit: https://www.virtualdub.org/blog2/entry_366.html
    stream << "#define __D3D9_OFFSET_X " << (-1.0f / Minecraft::width) << "\n";
    stream << "#define __D3D9_OFFSET_Y " << (1.0f / Minecraft::height) << "\n";
}
