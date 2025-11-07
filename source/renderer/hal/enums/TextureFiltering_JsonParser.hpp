#pragma once

#include "common/utility/JsonParser.hpp"
#include "TextureFiltering.hpp"

namespace mce
{
    const std::map<std::string, TextureFiltering> _textureFilterMap = {
        {"Point", TEXTURE_FILTERING_POINT},
        {"Bilinear", TEXTURE_FILTERING_BILINEAR},
        {"Trilinear", TEXTURE_FILTERING_TRILINEAR},
        {"MipMapBilinear", TEXTURE_FILTERING_MIPMAP_BILINEAR},
        {"TexelAA", TEXTURE_FILTERING_TEXEL_AA},
        {"PCF", TEXTURE_FILTERING_PCF}
    };

    template <>
    bool parse<TextureFiltering>(const rapidjson::Value& root, const std::string& name, TextureFiltering& out)
    {
        return parse<TextureFiltering>(root, name, _textureFilterMap, out);
    }
}