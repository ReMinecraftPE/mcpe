#pragma once

#include <map>
#include <string>
#include "RenderState.hpp"

namespace mce
{
    std::map<std::string, RenderState> _CreateRenderStateMap()
    {
        std::map<std::string, RenderState> m;
        m["DisableDepthTest"] = RS_DISABLE_DEPTH_TEST;
        m["Blending"] = RS_BLENDING;
        m["PolygonOffset"] = RS_POLYGON_OFFSET;
        m["DisableCulling"] = RS_DISABLE_CULLING;
        m["DisableColorWrite"] = RS_DISABLE_COLOR_WRITE;
        m["DisableDepthWrite"] = RS_DISABLE_DEPTH_WRITE;
        m["StencilWrite"] = RS_STENCIL_WRITE;
        m["InvertCulling"] = RS_INVERT_CULLING;
        m["EnableStencilTest"] = RS_ENABLE_STENCIL_TEST;
        m["EnableAlphaTest"] = RS_ENABLE_ALPHA_TEST;
        m["Textured"] = RS_ENABLE_TEXTURE;
        return m;
    }
    const std::map<std::string, RenderState> _renderStateMap = _CreateRenderStateMap();
}
