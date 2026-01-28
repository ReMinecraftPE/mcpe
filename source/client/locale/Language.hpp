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
    bool contains(const std::string& key) const;
    const std::string& get(const std::string& key) const;

    static Language* getInstance() 
    {
        return instance;
    }

private:
    std::map<std::string, std::string> m_translations;
    static Language* instance;
};
