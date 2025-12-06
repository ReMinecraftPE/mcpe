#include "common/utility/JsonParser.hpp"
#include "PrimitiveMode.hpp"

namespace mce
{

	std::map<std::string, PrimitiveMode> _CreatePrimitiveModeMap()
	{
		std::map<std::string, PrimitiveMode> m;
		m["None"]          = PRIMITIVE_MODE_NONE;
		m["QuadList"]      = PRIMITIVE_MODE_QUAD_LIST;
		m["TriangleList"]  = PRIMITIVE_MODE_TRIANGLE_LIST;
		m["TriangleStrip"] = PRIMITIVE_MODE_TRIANGLE_STRIP;
		m["LineList"]      = PRIMITIVE_MODE_LINE_LIST;
		m["Line"]          = PRIMITIVE_MODE_LINE_STRIP;
		return m;
	}

	const std::map<std::string, PrimitiveMode> _primitiveModeMap = _CreatePrimitiveModeMap();

	template <>
	bool parse<PrimitiveMode>(const rapidjson::Value& root, const std::string& name, PrimitiveMode& out)
	{
		return parse<PrimitiveMode>(root, name, _primitiveModeMap, out);
	}
	
}
