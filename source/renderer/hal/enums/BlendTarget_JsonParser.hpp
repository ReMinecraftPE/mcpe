#pragma once

#include "common/utility/JsonParser.hpp"
#include "BlendTarget.hpp"

namespace mce
{
    const std::map<std::string, BlendTarget> _blendTargetMap = {
        {"DestColor", BLEND_TARGET_DEST_COLOR},
        {"SourceColor", BLEND_TARGET_SOURCE_COLOR},
        {"Zero", BLEND_TARGET_ZERO},
        {"One", BLEND_TARGET_ONE},
        {"OneMinusDestColor", BLEND_TARGET_ONE_MINUS_DEST_COLOR},
        {"OneMinusSrcColor", BLEND_TARGET_ONE_MINUS_SRC_COLOR},
        {"SourceAlpha", BLEND_TARGET_SOURCE_ALPHA},
        {"DestAlpha", BLEND_TARGET_DEST_ALPHA},
        {"OneMinusSrcAlpha", BLEND_TARGET_ONE_MINUS_SRC_ALPHA}
    };

    template <>
    bool parse<BlendTarget>(const rapidjson::Value& root, const std::string& name, BlendTarget& out)
    {
        return parse<BlendTarget>(root, name, _blendTargetMap, out);
    }
}