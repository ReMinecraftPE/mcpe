#include "common/utility/JsonParser.hpp"
#include "RenderState_JsonParser.hpp"

namespace mce
{

	template <>
	bool parse<RenderState>(const rapidjson::Value& root, const std::string& name, RenderState& out)
	{
		return parse<RenderState>(root, name, _renderStateMap, out);
	}

}
