#pragma once

#include <set>
#include <string>
#include <stdint.h>

#include "thirdparty/rapidjson/document.h"

#include "hal/enums/RenderState.hpp"
#include "hal/interface/Shader.hpp"
#include "hal/interface/BlendState.hpp"
#include "hal/interface/DepthStencilState.hpp"
#include "hal/interface/RasterizerState.hpp"

#include "ShaderGroup.hpp"

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

    public:
        RenderMaterial();
        RenderMaterial(const rapidjson::Value& root, const RenderMaterial& parent);

    protected:
        RenderState _parseStateName(const std::string& stateName) const;
        void _parseRenderStates(const rapidjson::Value& root);
        void _parseRuntimeStates(const rapidjson::Value& root);
        void _parseDepthStencilFace(const rapidjson::Value& root, const char* depthStencilFaceName, StencilFaceDescription& faceDescription) const;
        void _parseDepthStencilState(const rapidjson::Value& root);
        void _parseBlendState(const rapidjson::Value& root);
#if FEATURE_GFX_SHADERS
        void _parseShaderPaths(const rapidjson::Value& root);
        void _parseDefines(const rapidjson::Value& root);
        std::string _buildHeader();
        void _loadShader(ShaderGroup& shaderGroup);
#endif
        void _applyRenderStates();

    public:
        void useWith(RenderContext& context, const VertexFormat& vertexFormat, const void *basePtr);
        void addState(RenderState state);
        bool hasState(RenderState state) const { return (m_stateMask & (1 << state)) != 0; }

    protected:
        static void SpliceShaderPath(std::string& shaderName);
    public:
        static void InitContext();
    };
}