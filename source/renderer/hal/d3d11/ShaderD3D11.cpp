#include <typeinfo>
#include <stdexcept>

#include "ShaderD3D11.hpp"

#include "common/Util.hpp"
#include "renderer/GlobalConstantBufferManager.hpp"
#include "renderer/ConstantBufferMetaDataManager.hpp"
#include "renderer/RenderContextImmediate.hpp"
#include "renderer/hal/dxgi/helpers/ErrorHandlerDXGI.hpp"

using namespace mce;

ShaderD3D11::ShaderD3D11(ShaderProgram& vertex, ShaderProgram& fragment, ShaderProgram& geometry)
    : ShaderBase(vertex, fragment, geometry)
{
    m_bCompiledShaders = false;
}

ShaderD3D11::~ShaderD3D11()
{
}

DXGI_FORMAT formatFromVertexField(VertexField vertexField)
{
    const RenderContext& context = RenderContextImmediate::getAsConst();

    switch (vertexField)
    {
    case VERTEX_FIELD_POSITION: return DXGI_FORMAT_R32G32B32_FLOAT;
    case VERTEX_FIELD_COLOR:    return DXGI_FORMAT_R8G8B8A8_UNORM;
    case VERTEX_FIELD_NORMAL:   return context.supportsR8G8B8A8_SNORM() ? DXGI_FORMAT_R8G8B8A8_SNORM : DXGI_FORMAT_R8G8B8A8_UNORM;
#ifdef ENH_GFX_COMPACT_UVS
    case VERTEX_FIELD_UV0:
    case VERTEX_FIELD_UV1:      return context.supportsR16G16_UNORM() ? DXGI_FORMAT_R16G16_UNORM : DXGI_FORMAT_R16G16_SNORM;
#else
    case VERTEX_FIELD_UV0:
    case VERTEX_FIELD_UV1:      return DXGI_FORMAT_R32G32_FLOAT;
#endif
    default:
        LOG_W("Unknown vertexField: %d", vertexField);
        return DXGI_FORMAT_UNKNOWN;
    }
}

ShaderResourceD3D11* ShaderD3D11::_getShaderResource(const std::string& name)
{
    for (std::vector<ShaderResourceD3D11>::iterator it = m_resourceList.begin(); it != m_resourceList.end(); it++)
    {
        if (it->m_pConstantBufferContainer && it->m_pConstantBufferContainer->getConstantBufferName() == name)
        {
            return &*it;
        }
    }

    return nullptr;
}

void ShaderD3D11::bindVertexPointers(RenderContext& context, const VertexFormat& vertexFormat)
{
    RenderContextD3D11::InputLayoutID inputLayoutId(vertexFormat, m_attributeListIndex);
    if (inputLayoutId.vertexFormat       == context.m_lastVertexFormat
     && inputLayoutId.attributeListIndex == context.m_lastAttributeListIndex)
    {
        return;
    }

    ComInterface<ID3D11InputLayout> inputLayout;
    {
        RenderContextD3D11::InputLayoutCache::iterator inputLayoutCacheItr = context.m_inputLayoutCache.find(inputLayoutId);
        if (inputLayoutCacheItr == context.m_inputLayoutCache.end())
        {
            inputLayout = createInputLayout(vertexFormat);
            context.m_inputLayoutCache[inputLayoutId] = inputLayout;
        }
        else
        {
            inputLayout = inputLayoutCacheItr->second;
        }
    }

    {
        D3DDeviceContext d3dDeviceContext = context.getD3DDeviceContext();
        d3dDeviceContext->IASetInputLayout(**inputLayout);
    }

    context.m_lastVertexFormat = inputLayoutId.vertexFormat;
    context.m_lastAttributeListIndex = inputLayoutId.attributeListIndex;
}

void ShaderD3D11::bindShader(RenderContext& context, const VertexFormat& format, const void* dataBasePtr, unsigned int shaderStageBits)
{
    if (!m_bCompiledShaders)
        compileAndLinkShader();

    bindVertexPointers(context, format);

    {
        D3DDeviceContext d3dDeviceContext = context.getD3DDeviceContext();

        if (context.m_lastShaderPrograms[SHADER_TYPE_VERTEX] != &m_vertexShader)
        {
            d3dDeviceContext->VSSetShader(**m_vertexShader.m_vertexShader, NULL, 0);
            context.m_lastShaderPrograms[SHADER_TYPE_VERTEX] = &m_vertexShader;
        }

        if (context.m_lastShaderPrograms[SHADER_TYPE_FRAGMENT] != &m_fragmentShader)
        {
            d3dDeviceContext->PSSetShader(**m_fragmentShader.m_pixelShader, NULL, 0);
            context.m_lastShaderPrograms[SHADER_TYPE_FRAGMENT] = &m_fragmentShader;
        }

        if (m_geometryShader.isValid() && context.m_lastShaderPrograms[SHADER_TYPE_GEOMETRY] != &m_geometryShader)
        {
            d3dDeviceContext->GSSetShader(**m_geometryShader.m_geometryShader, NULL, 0);
            context.m_lastShaderPrograms[SHADER_TYPE_GEOMETRY] = &m_geometryShader;
        }
    }

    for (int i = 0; i < m_resourceList.size(); i++)
    {
        for (int t = 0; t < SHADER_TYPES_COUNT; t++)
        {
            ShaderResourceD3D11& resource = m_resourceList[i];
            if (((1 << t) & resource.m_shaderStagesBits) != 0)
            {
                resource.m_pConstantBufferContainer->bindBuffer(context, resource.m_shaderBindPoints[t], resource.m_shaderStagesBits);
            }
        }
    }
}

void ShaderD3D11::compileAndLinkShader()
{
    if (m_bCompiledShaders)
        return;

    m_vertexShader.compileShaderProgram();
    m_fragmentShader.compileShaderProgram();
    m_geometryShader.compileShaderProgram();

    reflectShader(m_vertexShader,   SHADER_TYPE_VERTEX);
    reflectShader(m_fragmentShader, SHADER_TYPE_FRAGMENT);

    if (m_geometryShader.isValid())
        reflectShader(m_geometryShader, SHADER_TYPE_GEOMETRY);

    m_bCompiledShaders = true;
}

void ShaderD3D11::reflectShaderResources(ComInterface<ID3D11ShaderReflection> shaderReflection, const D3D11_SHADER_DESC& shaderDesc, ShaderType shaderType)
{
    GlobalConstantBufferManager& bufferManager = GlobalConstantBufferManager::getInstance();

    unsigned int shaderStagesBits = 1 << shaderType;

    for (int i = 0; i < shaderDesc.ConstantBuffers; i++)
    {
        ID3D11ShaderReflectionConstantBuffer* constantBuffer = shaderReflection->GetConstantBufferByIndex(i);

        D3D11_SHADER_BUFFER_DESC bufferDesc;
        constantBuffer->GetDesc(&bufferDesc);

        D3D11_SHADER_INPUT_BIND_DESC shaderInputBindDesc;
        shaderReflection->GetResourceBindingDescByName(bufferDesc.Name, &shaderInputBindDesc);

        ShaderResourceD3D11* pShaderResource = _getShaderResource(bufferDesc.Name);

        if (pShaderResource)
        {
            pShaderResource->m_shaderBindPoints[shaderType] = shaderInputBindDesc.BindPoint;
            pShaderResource->m_shaderStagesBits |= shaderStagesBits;

            // @HAL: does nothing. for fun?
            for (int i = 0; i < bufferDesc.Variables; i++)
            {
                ID3D11ShaderReflectionVariable* shaderVariable = constantBuffer->GetVariableByIndex(i);

                D3D11_SHADER_VARIABLE_DESC shaderVariableDesc;
                shaderVariable->GetDesc(&shaderVariableDesc);

                pShaderResource->m_pConstantBufferContainer->getUnspecializedShaderConstant(shaderVariableDesc.Name);
            }
        }
        else
        {
            ConstantBufferMetaDataManager& metaDataManager = ConstantBufferMetaDataManager::getInstance();
            metaDataManager.findConstantBuffer(bufferDesc.Name);

            // @HAL: does nothing. for fun?
            for (int i = 0; i < bufferDesc.Variables; i++)
            {
                ID3D11ShaderReflectionVariable* shaderVariable = constantBuffer->GetVariableByIndex(i);

                D3D11_SHADER_VARIABLE_DESC shaderVariableDesc;
                shaderVariable->GetDesc(&shaderVariableDesc);

                ID3D11ShaderReflectionType* type = shaderVariable->GetType();

                D3D11_SHADER_TYPE_DESC shaderTypeDesc;
                type->GetDesc(&shaderTypeDesc);
            }

            ConstantBufferContainer* pConstantBufferContainer = bufferManager.findConstantBufferContainer(bufferDesc.Name);

            ShaderResourceD3D11 shaderResource;
            {
                shaderResource.m_shaderBindPoints[shaderType] = shaderInputBindDesc.BindPoint;
                shaderResource.m_pConstantBufferContainer = pConstantBufferContainer;
                shaderResource.m_shaderStagesBits |= shaderStagesBits;
            }

            m_resourceList.push_back(shaderResource);
        }
    }
}

void ShaderD3D11::reflectShaderAttributes(ComInterface<ID3D11ShaderReflection> shaderReflection, const D3D11_SHADER_DESC& shaderDesc)
{
    RenderDeviceBase::AttributeList attrList;
    D3D11_SIGNATURE_PARAMETER_DESC paramDesc;

    for (int i = 0; i < shaderDesc.InputParameters; i++)
    {
        shaderReflection->GetInputParameterDesc(i, &paramDesc);
        // Check if semantic name begins with "SV_"
        if (strstr(paramDesc.SemanticName, "SV_") != paramDesc.SemanticName)
        {
            std::string semanticName(paramDesc.SemanticName);
            VertexField vertexField = getAttributeForName(semanticName, paramDesc.SemanticIndex);
            attrList.push_back(Attribute(i, 1, vertexField));
        }
    }

    m_attributeListIndex = RenderDevice::getInstance().registerOrGetAttributeListIndex(attrList);
}

void ShaderD3D11::reflectShader(const ShaderProgramD3D11& shaderProgram, ShaderType shaderType)
{
    if (!shaderProgram.isValid())
    {
        LOG_E("Tried to reflect invalid ShaderProgram. Are you missing a shader file?");
        throw std::bad_cast();
    }
    const std::string& shaderBytecode = shaderProgram.m_shaderBytecode;
    assert(!shaderBytecode.empty());
    ComInterface<ID3D11ShaderReflection> reflector;
    {
        void* pReflector;
        HRESULT hResult = D3DReflect(shaderBytecode.c_str(), shaderBytecode.size(), IID_ID3D11ShaderReflection, &pReflector);
        ErrorHandlerDXGI::checkForErrors(hResult);
        reflector = (ID3D11ShaderReflection*)pReflector;
    }

    D3D11_SHADER_DESC shaderDesc;
    reflector->GetDesc(&shaderDesc);

    if (shaderType == SHADER_TYPE_VERTEX)
        reflectShaderAttributes(reflector, shaderDesc);
    reflectShaderResources(reflector, shaderDesc, shaderType);
    reflectBindableResources(reflector, shaderDesc);
}

// @HAL: does nothing. for fun?
void ShaderD3D11::reflectBindableResources(ComInterface<ID3D11ShaderReflection> shaderReflection, const D3D11_SHADER_DESC& shaderDesc)
{
    if (!shaderDesc.BoundResources)
        return;

    D3D11_SHADER_INPUT_BIND_DESC shaderInputBindDescription;

    for (int i = 0; i < 1; i++)
    {
        shaderReflection->GetResourceBindingDesc(i, &shaderInputBindDescription);
        switch (shaderInputBindDescription.Type)
        {
        case D3D_SIT_CBUFFER:
        case D3D_SIT_TEXTURE:
        case D3D_SIT_SAMPLER:
            break;
        default:
            LOG_W("Unknown shaderInputBindDescription.Type: %d", shaderInputBindDescription.Type);
            break;
        }
    }
}

ComInterface<ID3D11InputLayout> ShaderD3D11::createInputLayout(const VertexFormat& vertexFormat)
{
    RenderDevice& renderDevice = RenderDevice::getInstance();
    RenderContext& renderContext = renderDevice.getRenderContext();

    RenderDeviceBase::AttributeList& attrList = renderDevice.m_registeredAttributeLists[m_attributeListIndex];
    std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescriptions;

    // keeps the c-strings in memory until input layouts can be created for them
    std::vector<std::string> attrNames;
    attrNames.resize(attrList.size());

    for (int i = 0; i < attrList.size(); i++)
    {
        Attribute& attr = attrList[i];
        std::string& elementName = attrNames[attr.getLocation()];
        elementName = vertexFieldNames[attr.getVertexField()];
        const void* elementOffset = vertexFormat.getFieldOffset(attr.getVertexField());

        D3D11_INPUT_ELEMENT_DESC elementDesc = {0};
        {
            elementDesc.Format = formatFromVertexField(attr.getVertexField());
            elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            elementDesc.AlignedByteOffset = (UINT)elementOffset;

            unsigned int index = 0;

            size_t separatorPos = elementName.rfind('_');
            if (separatorPos != std::string::npos)
            {
                index = strtol(elementName.c_str() + separatorPos + 1, NULL, 16);
                elementName = elementName.substr(0, separatorPos + 1);
            }

            elementDesc.SemanticIndex = index;
            elementDesc.SemanticName = elementName.c_str();
        }
        inputElementDescriptions.push_back(elementDesc);
    }

    const std::string& shaderBytecode = m_vertexShader.m_shaderBytecode;
    ID3D11InputLayout* pInputLayout;

    D3DDevice d3dDevice = renderContext.getD3DDevice();
    HRESULT hResult = d3dDevice->CreateInputLayout(
        &inputElementDescriptions.front(),
        inputElementDescriptions.size(),
        shaderBytecode.c_str(),
        shaderBytecode.size(),
        &pInputLayout);
    ErrorHandlerDXGI::checkForErrors(hResult);

    return ComInterface<ID3D11InputLayout>(pInputLayout);
}

void ShaderD3D11::SpliceShaderPath(std::string& shaderName)
{
    ShaderBase::SpliceShaderPath(shaderName, "/hlsl", ".hlsl");
}

void ShaderD3D11::BuildHeader(std::ostringstream& stream)
{
    const RenderContext& renderContext = RenderContextImmediate::getAsConst();

    if (!renderContext.supportsR8G8B8A8_SNORM())
        stream << "#define R8G8B8A8_SNORM_UNSUPPORTED";
}
