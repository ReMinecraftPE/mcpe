#pragma once

#include <string>
#include <map>

#include "thirdparty/rapidjson/document.h"

namespace mce
{
    template <typename T>
    bool parse(const rapidjson::Value& root, const std::string& name, const std::map<std::string, T>& map, T& out)
    {
        if (root.IsNull())
            return false;

        const rapidjson::Value& value = root[name.c_str()];
        if (value.IsNull())
            return false;

        std::string valueStr = value.GetString();
        out = map.at(valueStr);

        return true;
    }
    
    template <typename T>
    bool parse(const rapidjson::Value& root, const std::string& name, T& out);
}