#include "common/utility/JsonParser.hpp"
#include "RenderStateType_JsonParser.hpp"

namespace mce
{

	template <>
	bool parse<RenderStateType>(const rapidjson::Value& root, const std::string& name, RenderStateType& out)
	{
		return parse<RenderStateType>(root, name, _renderStateTypeMap, out);
	}

}
