#pragma once

namespace mce
{
    enum StencilOp
    {
        STENCIL_OP_KEEP = 0x1,
        STENCIL_OP_ZERO = 0x2,
        STENCIL_OP_REPLACE = 0x3,
        STENCIL_OP_INCR = 0x7,
        STENCIL_OP_DECR = 0x8
    };
}
