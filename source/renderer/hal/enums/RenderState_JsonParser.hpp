#pragma once

#include "common/utility/JsonParser.hpp"
#include "RenderState.hpp"

namespace mce
{
    const std::map<std::string, RenderState> _renderStateMap = {
        {"DisableDepthTest", RS_DISABLE_DEPTH_TEST},
        {"Blending", RS_BLENDING},
        {"PolygonOffset", RS_POLYGON_OFFSET},
        {"DisableCulling", RS_DISABLE_CULLING},
        {"DisableColorWrite", RS_DISABLE_COLOR_WRITE},
        {"DisableDepthWrite", RS_DISABLE_DEPTH_WRITE},
        {"StencilWrite", RS_STENCIL_WRITE},
        {"InvertCulling", RS_INVERT_CULLING},
        {"EnableStencilTest", RS_ENABLE_STENCIL_TEST},
    };

    template <>
    bool parse<RenderState>(const rapidjson::Value& root, const std::string& name, RenderState& out)
    {
        return parse<RenderState>(root, name, _renderStateMap, out);
    }
}