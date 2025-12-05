#include "common/utility/JsonParser.hpp"
#include "ShaderStagesBits.hpp"

using namespace mce;

std::map<std::string, ShaderStagesBits> _CreateShaderStageMap()
{
	std::map<std::string, ShaderStagesBits> m;
    m["Vertex"]   = SHADER_STAGE_BIT_VERTEX;
    m["Pixel"]    = SHADER_STAGE_BIT_PIXEL;
    m["Geometry"] = SHADER_STAGE_BIT_GEOMETRY;
	return m;
}
const std::map<std::string, ShaderStagesBits> _shaderStageMap = _CreateShaderStageMap();

template <>
bool mce::parse<ShaderStagesBits>(const rapidjson::Value& root, const std::string& name, ShaderStagesBits& out)
{
    return parse<ShaderStagesBits>(root, name, _shaderStageMap, out);
}
