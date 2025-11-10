#include "common/utility/JsonParser.hpp"
#include "ShaderStagesBits.hpp"

using namespace mce;

const std::map<std::string, ShaderStagesBits> _shaderStageMap = {
    {"Vertex", SHADER_STAGE_BIT_VERTEX},
    {"Pixel", SHADER_STAGE_BIT_PIXEL},
    {"Geometry", SHADER_STAGE_BIT_GEOMETRY}
};

template <>
bool mce::parse<ShaderStagesBits>(const rapidjson::Value& root, const std::string& name, ShaderStagesBits& out)
{
    return parse<ShaderStagesBits>(root, name, _shaderStageMap, out);
}
