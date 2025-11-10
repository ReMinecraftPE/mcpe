#include "common/utility/JsonParser.hpp"
#include "VertexField.hpp"

using namespace mce;

const std::map<std::string, VertexField> _vertexFieldMap = {
    {"Position", VERTEX_FIELD_POSITION},
    {"Color", VERTEX_FIELD_COLOR},
    {"Normal", VERTEX_FIELD_NORMAL},
    {"UV0", VERTEX_FIELD_UV0},
    {"UV1", VERTEX_FIELD_UV1},
    {"UV2", VERTEX_FIELD_UV2},
    {"PBRTextureIdx", VERTEX_FIELD_PBR_IDX},
    {"BoneId0", VERTEX_FIELD_BONEID_0},
    {"PreviousPosition", VERTEX_FIELD_PREVIOUS_POSITION},
    {"HummingbirdPosition", VERTEX_FIELD_HB_POSITION},
    {"HummingbirdColor", VERTEX_FIELD_HB_COLOR},
    {"HummingbirdAdditional", VERTEX_FIELD_HB_ADDITIONAL}
};

template <>
bool mce::parse<VertexField>(const rapidjson::Value& root, const std::string& name, VertexField& out)
{
    return parse<VertexField>(root, name, _vertexFieldMap, out);
}
