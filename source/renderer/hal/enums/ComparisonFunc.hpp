#pragma once

namespace mce
{
    // Previously called DepthStencilFunc
    enum ComparisonFunc
    {
        COMPARISON_FUNC_EQUAL,
        COMPARISON_FUNC_NOT_EQUAL,
        COMPARISON_FUNC_ALWAYS,
        COMPARISON_FUNC_LESS,
        COMPARISON_FUNC_GREATER,
        COMPARISON_FUNC_GREATER_EQUAL,
        COMPARISON_FUNC_LESS_EQUAL
    };
}
