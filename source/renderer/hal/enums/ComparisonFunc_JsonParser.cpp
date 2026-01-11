#include "common/utility/JsonParser.hpp"
#include "ComparisonFunc.hpp"

namespace mce
{

	std::map<std::string, ComparisonFunc> _CreateComparisonFunctMap()
	{
		std::map<std::string, ComparisonFunc> m;
		m["Always"]       = COMPARISON_FUNC_ALWAYS;
		m["Equal"]        = COMPARISON_FUNC_EQUAL;
		m["NotEqual"]     = COMPARISON_FUNC_NOT_EQUAL;
		m["Less"]         = COMPARISON_FUNC_LESS;
		m["Greater"]      = COMPARISON_FUNC_GREATER;
		m["GreaterEqual"] = COMPARISON_FUNC_GREATER_EQUAL;
		m["LessEqual"]    = COMPARISON_FUNC_LESS_EQUAL;
		return m;
	}
	const std::map<std::string, ComparisonFunc> _comparisonFunctMap = _CreateComparisonFunctMap();

	template <>
	bool parse<ComparisonFunc>(const rapidjson::Value& root, const std::string& name, ComparisonFunc& out)
	{
		return parse<ComparisonFunc>(root, name, _comparisonFunctMap, out);
	}

}
