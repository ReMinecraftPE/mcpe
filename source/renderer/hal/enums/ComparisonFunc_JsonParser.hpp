#pragma once

#include "common/utility/JsonParser.hpp"
#include "ComparisonFunc.hpp"

namespace mce
{
    const std::map<std::string, ComparisonFunc> _comparisonFunctMap = {
        {"Always", COMPARISON_FUNC_ALWAYS},
        {"Equal", COMPARISON_FUNC_EQUAL},
        {"NotEqual", COMPARISON_FUNC_NOT_EQUAL},
        {"Less", COMPARISON_FUNC_LESS},
        {"Greater", COMPARISON_FUNC_GREATER},
        {"GreaterEqual", COMPARISON_FUNC_GREATER_EQUAL},
        {"LessEqual", COMPARISON_FUNC_LESS_EQUAL}
    };

    template <>
    bool parse<ComparisonFunc>(const rapidjson::Value& root, const std::string& name, ComparisonFunc& out)
    {
        return parse<ComparisonFunc>(root, name, _comparisonFunctMap, out);
    }
}