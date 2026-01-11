#include "common/utility/JsonParser.hpp"
#include "BlendTarget.hpp"

namespace mce
{
	std::map<std::string, BlendTarget> _CreateBlendTargetMap()
	{
		std::map<std::string, BlendTarget> m;
		m["DestColor"]         = BLEND_TARGET_DEST_COLOR;
		m["SourceColor"]       = BLEND_TARGET_SOURCE_COLOR;
		m["Zero"]              = BLEND_TARGET_ZERO;
		m["One"]               = BLEND_TARGET_ONE;
		m["OneMinusDestColor"] = BLEND_TARGET_ONE_MINUS_DEST_COLOR;
		m["OneMinusSrcColor"]  = BLEND_TARGET_ONE_MINUS_SRC_COLOR;
		m["SourceAlpha"]       = BLEND_TARGET_SOURCE_ALPHA;
		m["DestAlpha"]         = BLEND_TARGET_DEST_ALPHA;
		m["OneMinusSrcAlpha"]  = BLEND_TARGET_ONE_MINUS_SRC_ALPHA;
		return m;
	}
	const std::map<std::string, BlendTarget> _blendTargetMap = _CreateBlendTargetMap();

	template <>
	bool parse<BlendTarget>(const rapidjson::Value& root, const std::string& name, BlendTarget& out)
	{
		return parse<BlendTarget>(root, name, _blendTargetMap, out);
	}
}
