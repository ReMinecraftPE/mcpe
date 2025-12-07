#include "common/utility/JsonParser.hpp"
#include "VertexField.hpp"

namespace mce
{
	std::map<std::string, VertexField> _CreateVertexFieldMap()
	{
		std::map<std::string, VertexField> m;
		m["Position"]              = VERTEX_FIELD_POSITION;
		m["Color"]                 = VERTEX_FIELD_COLOR;
		m["Normal"]                = VERTEX_FIELD_NORMAL;
		m["UV0"]                   = VERTEX_FIELD_UV0;
		m["UV1"]                   = VERTEX_FIELD_UV1;
		m["UV2"]                   = VERTEX_FIELD_UV2;
		m["PBRTextureIdx"]         = VERTEX_FIELD_PBR_IDX;
		m["BoneId0"]               = VERTEX_FIELD_BONEID_0;
		m["PreviousPosition"]      = VERTEX_FIELD_PREVIOUS_POSITION;
		m["HummingbirdPosition"]   = VERTEX_FIELD_HB_POSITION;
		m["HummingbirdColor"]      = VERTEX_FIELD_HB_COLOR;
		m["HummingbirdAdditional"] = VERTEX_FIELD_HB_ADDITIONAL;
		return m;
	}
	const std::map<std::string, VertexField> _vertexFieldMap = _CreateVertexFieldMap();

	template <>
	bool parse<VertexField>(const rapidjson::Value& root, const std::string& name, VertexField& out)
	{
		return parse<VertexField>(root, name, _vertexFieldMap, out);
	}
}
