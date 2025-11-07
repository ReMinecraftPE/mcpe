#pragma once

namespace mce
{
    enum RenderState
    {
        RS_NONE,
        RS_DISABLE_DEPTH_TEST,
        RS_BLENDING,
        RS_POLYGON_OFFSET,
        RS_DISABLE_CULLING,
        RS_DISABLE_COLOR_WRITE,
        RS_DISABLE_DEPTH_WRITE,
        RS_STENCIL_WRITE,
        RS_INVERT_CULLING,
        RS_ENABLE_STENCIL_TEST
    };
}