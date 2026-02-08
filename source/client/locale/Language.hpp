#pragma once

#include <map>
#include <string>

class Options;

class Language
{
public:
    void init(Options*);
    bool loadLanguageFile(const std::string& path);
    bool loadOriginalLanguageFile(const std::string& path);
    bool has(const std::string& key) const;
    const std::string& getOrDefault(const std::string& key) const;

    static const std::string& get(const std::string& key) { return singleton().getOrDefault(key); }

    static Language& singleton()
    {
        if (!instance)
        {
            instance = new Language;
        }
        return *instance;
    }


private:
    std::map<std::string, std::string> m_translations;
    static Language* instance;
};
