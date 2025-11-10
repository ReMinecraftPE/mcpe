#include "common/utility/JsonParser.hpp"
#include "PrimitiveMode.hpp"

using namespace mce;

const std::map<std::string, PrimitiveMode> _primitiveModeMap = {
    {"None", PRIMITIVE_MODE_NONE},
    {"QuadList", PRIMITIVE_MODE_QUAD_LIST},
    {"TriangleList", PRIMITIVE_MODE_TRIANGLE_LIST},
    {"TriangleStrip", PRIMITIVE_MODE_TRIANGLE_STRIP},
    {"LineList", PRIMITIVE_MODE_LINE_LIST},
    {"Line", PRIMITIVE_MODE_LINE_STRIP}
};

template <>
bool mce::parse<PrimitiveMode>(const rapidjson::Value& root, const std::string& name, PrimitiveMode& out)
{
    return parse<PrimitiveMode>(root, name, _primitiveModeMap, out);
}
