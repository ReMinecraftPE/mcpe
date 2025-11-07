#pragma once

#include "common/utility/JsonParser.hpp"
#include "StencilOp.hpp"

namespace mce
{
    const std::map<std::string, StencilOp> _stencilOpMap = {
        {"Keep", STENCIL_OP_KEEP},
        {"Replace", STENCIL_OP_REPLACE}
    };

    template <>
    bool parse<StencilOp>(const rapidjson::Value& root, const std::string& name, StencilOp& out)
    {
        return parse<StencilOp>(root, name, _stencilOpMap, out);
    }
}