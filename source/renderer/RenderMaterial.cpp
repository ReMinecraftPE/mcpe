#include "RenderMaterial.hpp"

#include "PlatformDefinitions.h"

#include "common/utility/JsonParser.hpp"

#include "hal/enums/RenderState_JsonParser.hpp"
#include "hal/enums/ComparisonFunc.hpp"
#include "hal/enums/StencilOp.hpp"
#include "hal/enums/ShaderStagesBits.hpp"
#include "RenderContextImmediate.hpp"

#include "EnableScissorTest.hpp"

#if MCE_GFX_API_OGL
#include "platform/ogl/ShaderPrecision.hpp"
#endif

using namespace mce;

RenderMaterial* RenderMaterial::lastUsedMaterial = nullptr;

RenderMaterial::RenderMaterial()
{
    m_stateMask = 0;
    m_polygonOffsetLevel = -2.0f;
    m_pShader = nullptr;
}

RenderMaterial::RenderMaterial(const rapidjson::Value& root, const RenderMaterial& parent)
{
	*this = parent;
    _parseRenderStates(root);
    _parseRuntimeStates(root);

#ifdef FEATURE_SHADERS
    _parseShaderPaths(root);
    if (!m_vertexShader.empty() && !m_fragmentShader.empty())
    {
        _parseDefines(root);
        _loadShader(*ShaderGroup::singleton());
    }
#endif

    _applyRenderStates();

    RenderContext& renderContext = RenderContextImmediate::get();
    m_blendState.createBlendState(renderContext, m_blendStateDescription);
    m_depthStencilState.createDepthState(renderContext, m_depthStencilStateDescription);
    m_rasterizerState.createRasterizerStateDescription(renderContext, m_rasterizerStateDescription);
}

RenderState RenderMaterial::_parseStateName(const std::string& stateName) const
{
    return _renderStateMap.at(stateName);
}

void RenderMaterial::_parseRenderStates(const rapidjson::Value& root)
{
    if (!root.HasMember("states"))
        return;

    const rapidjson::Value& statesValue = root["states"];
    for (rapidjson::Value::ConstValueIterator it = statesValue.Begin(); it != statesValue.End(); it++)
    {
        std::string stateName = it->GetString();
        RenderState state = _parseStateName(stateName);
        addState(state);
    }
}

void RenderMaterial::_parseRuntimeStates(const rapidjson::Value& root)
{
    _parseDepthStencilState(root);
    _parseBlendState(root);

    if (root.HasMember("polygonOffsetLevel"))
    {
        const rapidjson::Value& polygonOffsetLevelValue = root["polygonOffsetLevel"];
        if (!polygonOffsetLevelValue.IsNull())
            m_polygonOffsetLevel = polygonOffsetLevelValue.GetFloat();
    }
}

void RenderMaterial::_parseDepthStencilFace(const rapidjson::Value& root, const char* depthStencilFaceName, StencilFaceDescription& faceDescription) const
{
    if (!root.HasMember("depthStencilFaceName"))
        return;

    const rapidjson::Value& value = root[depthStencilFaceName];
    if (value.IsNull())
        return;
    
    parse(value, "stencilFunc", faceDescription.stencilFunc);
    parse(value, "stencilFailOp", faceDescription.stencilFailOp);
    parse(value, "stencilDepthFailOp", faceDescription.stencilDepthFailOp);
    parse(value, "stencilPassOp", faceDescription.stencilPassOp);
}

void RenderMaterial::_parseDepthStencilState(const rapidjson::Value& root)
{
    DepthStencilStateDescription& desc = m_depthStencilStateDescription;
    
    parse(root, "depthFunc", desc.depthFunc);
    _parseDepthStencilFace(root, "frontFace", desc.frontFace);
    _parseDepthStencilFace(root, "backFace", desc.frontFace);

    if (root.HasMember("stencilRef"))
    {
        const rapidjson::Value& stencilRefValue = root["stencilRef"];
        if (!stencilRefValue.IsNull())
        {
            desc.overwroteStencilRef = true;
            desc.stencilRef = stencilRefValue.GetUint();
        }
    }

    if (root.HasMember("stencilReadMask"))
    {
        const rapidjson::Value& stencilReadMaskValue = root["stencilReadMask"];
        if (!stencilReadMaskValue.IsNull())
        {
            desc.stencilReadMask = stencilReadMaskValue.GetUint();
        }
    }
}

void RenderMaterial::_parseBlendState(const rapidjson::Value& root)
{
    parse(root, "blendSrc", m_blendStateDescription.blendSource);
    parse(root, "blendDst", m_blendStateDescription.blendDestination);
}

void RenderMaterial::_parseDefines(const rapidjson::Value& root)
{
    if (!root.HasMember("defines"))
        return;

    const rapidjson::Value& definesValue = root["defines"];
    for (rapidjson::Value::ConstValueIterator it = definesValue.Begin(); it != definesValue.End(); it++)
    {
        std::string defineStr = it->GetString();
        m_defines.insert(defineStr);
    }
}

std::string RenderMaterial::_buildHeader()
{
    std::string result; // Uses std::ostream in DX11

#if MCE_GFX_API_DX11
    // add R8G8B8A8_SNORM_UNSUPPORTED to defines if less than D3D_FEATURE_LEVEL_10_0
#endif

    for (std::set<std::string>::const_iterator it = m_defines.begin(); it != m_defines.end(); it++)
    {
        result += "#define " + *it + "\n";
    }

#if MCE_GFX_API_OGL
    result += Platform::OGL::Precision::buildHeader();
#endif

    return result;
}

void RenderMaterial::_parseShaderPaths(const rapidjson::Value& root)
{
    if (root.HasMember("vertexShader"))
    {
        const rapidjson::Value& pathValue = root["vertexShader"];
        if (!pathValue.IsNull())
            m_vertexShader = pathValue.GetString();
    }
    if (root.HasMember("fragmentShader"))
    {
        const rapidjson::Value& pathValue = root["fragmentShader"];
        if (!pathValue.IsNull())
            m_fragmentShader = pathValue.GetString();
    }
    if (root.HasMember("geometryShader"))
    {
        const rapidjson::Value& pathValue = root["geometryShader"];
        if (!pathValue.IsNull())
            m_geometryShader = pathValue.GetString();
    }
}

void RenderMaterial::_loadShader(ShaderGroup& shaderGroup)
{
#if MCE_GFX_API_DX11
    SpliceShaderPath(m_vertexShader);
    SpliceShaderPath(m_fragmentShader);
    SpliceShaderPath(m_geometryShader);
#endif
    std::string header = _buildHeader();
    m_pShader = &shaderGroup.loadShader(header, m_vertexShader, m_fragmentShader, m_geometryShader);
}

void RenderMaterial::_applyRenderStates()
{
    ColorWriteMask colorWriteMask;
    if (hasState(RS_DISABLE_COLOR_WRITE))
        colorWriteMask = COLOR_WRITE_MASK_NONE;
    else
        colorWriteMask = COLOR_WRITE_MASK_ALL;

    CullMode cullMode;
    if (hasState(RS_INVERT_CULLING))
        cullMode = CULL_FRONT;
    else
        cullMode = CULL_BACK;
    
    if (hasState(RS_DISABLE_CULLING))
        cullMode = CULL_NONE;

    m_depthStencilStateDescription.depthTestEnabled = !hasState(RS_DISABLE_DEPTH_TEST);
    m_depthStencilStateDescription.depthWriteMask = hasState(RS_DISABLE_DEPTH_WRITE) ? DEPTH_WRITE_MASK_NONE : DEPTH_WRITE_MASK_ALL;
    m_depthStencilStateDescription.stencilTestEnabled = hasState(RS_ENABLE_STENCIL_TEST);
    m_blendStateDescription.enableBlend = hasState(RS_BLENDING);

    float polygonOffsetLevel = 0.0f;
    if (hasState(RS_POLYGON_OFFSET))
        polygonOffsetLevel = m_polygonOffsetLevel;

    m_blendStateDescription.colorWriteMask = colorWriteMask;
    m_rasterizerStateDescription.cullMode = cullMode;
    m_rasterizerStateDescription.depthBias = polygonOffsetLevel;
}

void RenderMaterial::useWith(RenderContext& context, const VertexFormat& vertexFormat, const void *basePtr)
{
    m_blendState.bindBlendState(context);

    if (EnableScissorTest::scissorTestEnabled)
    {
        RasterizerStateDescription rasterizerDesc;
        rasterizerDesc = m_rasterizerStateDescription;
        rasterizerDesc.enableScissorTest = true;
        
        RasterizerState rasterizerState;
        rasterizerState.createRasterizerStateDescription(context, rasterizerDesc);
        rasterizerState.bindRasterizerState(context);
        rasterizerState.setScissorRect(
            context,
            EnableScissorTest::activeScissorBox[0], EnableScissorTest::activeScissorBox[1],
            EnableScissorTest::activeScissorBox[2], EnableScissorTest::activeScissorBox[3]
        );
    }
    else
    {
        m_rasterizerState.bindRasterizerState(context);
    }

    m_depthStencilState.bindDepthStencilState(context);

    lastUsedMaterial = this;

#ifdef FEATURE_SHADERS
    m_pShader->bindShader(context, vertexFormat, basePtr, SHADER_STAGE_BITS_ALL);
#endif
}

void RenderMaterial::addState(RenderState state)
{
    m_stateMask |= 1 << (state & 0x1F);
}

void RenderMaterial::SpliceShaderPath(std::string& shaderName)
{
    size_t shaderPathPos = shaderName.find_first_not_of("shaders");
    if (shaderPathPos != std::string::npos && shaderName.find(".hlsl") == std::string::npos)
    {
        shaderName.insert(shaderPathPos, "/dx11");
        shaderName.append(".hlsl");
    }
}

void RenderMaterial::initContext()
{
    lastUsedMaterial = nullptr;
}