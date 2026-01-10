#include "common/utility/JsonParser.hpp"
#include "TextureFiltering.hpp"

namespace mce
{

	std::map<std::string, TextureFiltering> _CreateTextureFilterMap()
	{
		std::map<std::string, TextureFiltering> m;
		m["Point"]          = TEXTURE_FILTERING_POINT;
		m["Bilinear"]       = TEXTURE_FILTERING_BILINEAR;
		m["Trilinear"]      = TEXTURE_FILTERING_TRILINEAR;
		m["MipMapBilinear"] = TEXTURE_FILTERING_MIPMAP_BILINEAR;
		m["TexelAA"]        = TEXTURE_FILTERING_TEXEL_AA;
		m["PCF"]            = TEXTURE_FILTERING_PCF;
		return m;
	}
	const std::map<std::string, TextureFiltering> _textureFilterMap = _CreateTextureFilterMap();

	template <>
	bool parse<TextureFiltering>(const rapidjson::Value& root, const std::string& name, TextureFiltering& out)
	{
		return parse<TextureFiltering>(root, name, _textureFilterMap, out);
	}

}
