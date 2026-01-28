#include "Language.hpp"
#include "thirdparty/rapidjson/document.h"
#include "client/options/Options.hpp"
#include "client/app/AppPlatform.hpp"
#include "client/resources/Resource.hpp"
#include <sstream>

Language* Language::instance = new Language();

void Language::init(Options* options)
{
    m_translations.clear();
    loadOriginalLanguageFile("lang/en_US.lang");
    loadOriginalLanguageFile("lang/stats_US.lang");

    //@NOTE: Should be used for custom lang entries
    loadLanguageFile("lang/en_us.json");
    //if (options->m_lang.get() != "en_us")
    //    loadLanguageFile("lang/" + options->m_lang.get() + ".json");
}

bool Language::loadLanguageFile(const std::string& path)
{
    using namespace rapidjson;

    std::string file;
    if (!Resource::load(path, file)) return false;
    Document d;
    d.Parse(file.c_str());

    if (d.ObjectEmpty()) return false;

    for (Value::ConstMemberIterator it = d.MemberBegin(); it != d.MemberEnd(); ++it)
    {
        if (it->value.IsString())
            m_translations[it->name.GetString()] = it->value.GetString();
    }

    return true;
}

bool Language::loadOriginalLanguageFile(const std::string& path)
{
    std::string file;
    if (!Resource::load(path, file)) return false;
    std::istringstream stream = std::istringstream(file);
    std::string line;

    while (std::getline(stream, line))
    {
        if (line.empty())
            continue;

        if (line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        std::istringstream lineStream = std::istringstream(line);
        std::vector<std::string> entry;
        std::string part;
        while (std::getline(lineStream, part, '='))
        {
            entry.push_back(part);
            if (entry.size() >= 2) break;
        }

        if (entry.size() < 2)
            continue;

        m_translations[entry[0]] = entry[1];
    }

    return true;
}

bool Language::contains(const std::string& key) const
{
    return m_translations.find(key) != m_translations.end();
}

const std::string& Language::get(const std::string& key) const
{
    std::map<std::string, std::string>::const_iterator it = m_translations.find(key);
    if (it != m_translations.end())
        return it->second;
    return key;
}
