#pragma once

#include <set>
#include <string>
#include <stdint.h>

#include "thirdparty/rapidjson/document.h"

#include "client/renderer/renderer/ShaderGroup.hpp"

#include "renderer/hal/enums/RenderStateType.hpp"
#include "renderer/hal/interface/Shader.hpp"
#include "renderer/hal/interface/BlendState.hpp"
#include "renderer/hal/interface/DepthStencilState.hpp"
#include "renderer/hal/interface/RasterizerState.hpp"
#include "renderer/hal/interface/RenderState.hpp"
#include "renderer/hal/interface/AlphaState.hpp"

namespace mce
{
    class RenderMaterial
    {
    public:
        static RenderMaterial* lastUsedMaterial;

    public:
        std::set<std::string> m_defines;
        int32_t m_stateMask;
        std::string m_vertexShader;
        std::string m_fragmentShader;
        std::string m_geometryShader;
        float m_polygonOffsetLevel;
        Shader *m_pShader; // we DO NOT own this, so don't delete it
        BlendState m_blendState;
        BlendStateDescription m_blendStateDescription;
        DepthStencilState m_depthStencilState;
        DepthStencilStateDescription m_depthStencilStateDescription;
        RasterizerState m_rasterizerState;
        RasterizerStateDescription m_rasterizerStateDescription;
        RenderState m_renderState;
        RenderStateDescription m_renderStateDescription;
        AlphaState m_alphaState;
        AlphaStateDescription m_alphaStateDescription;

    public:
        RenderMaterial();
        RenderMaterial(const rapidjson::Value::ConstObject& root, const RenderMaterial& parent);

    protected:
        RenderStateType _parseStateName(const std::string& stateName) const;
        void _parseRenderStates(const rapidjson::Value& root);
        void _parseRuntimeStates(const rapidjson::Value& root);
        void _parseDepthStencilFace(const rapidjson::Value& root, const char* depthStencilFaceName, StencilFaceDescription& faceDescription) const;
        void _parseDepthStencilState(const rapidjson::Value& root);
        void _parseBlendState(const rapidjson::Value& root);
        void _parseAlphaState(const rapidjson::Value& root);
        void _parseDefines(const rapidjson::Value& root);
        void _parseShaderPaths(const rapidjson::Value& root);
#ifdef FEATURE_GFX_SHADERS
        std::string _buildHeader();
        void _loadShader(ShaderGroup& shaderGroup);
#endif
        void _applyRenderStates();

    public:
        void useWith(RenderContext& context, const VertexFormat& vertexFormat, const void *basePtr);
        void compileShader();
        void addState(RenderStateType state);
        bool hasState(RenderStateType state) const { return (m_stateMask & (1 << state)) != 0; }
        
    public:
        static void InitContext();
    };
}
