#include "SoundPathRepository.hpp"

#include "common/Logger.hpp"
#include "common/Mth.hpp"

void SoundPathRepository::add(const std::string& name, const std::string& path)
{
    _all.push_back(path);

    std::map<std::string, std::vector<std::string> >::iterator iter = m_repo.find(name);
    if (iter == m_repo.end())
    {
        std::vector<std::string> paths;
        paths.push_back(path);
        m_repo.insert(std::make_pair(name, paths));
    }
    else
    {
        iter->second.push_back(path);
    }
}

bool SoundPathRepository::get(const std::string& name, std::string& path)
{
    // TODO: Who's the genius who decided it'd be better to check a name string rather than an enum?
    std::map<std::string, std::vector<std::string> >::iterator iter = m_repo.find(name);
    if (iter == m_repo.end())
    {
        LOG_E("Couldn't find a sound with id: %s", name.c_str());
        return false;
    }

    int index = Mth::random(int(iter->second.size()));

    path = iter->second[index];

    return true;
}

bool SoundPathRepository::any(std::string& path)
{
    if (_all.empty())
        return false;

    path = _all.at(Mth::random(_all.size()-1));
    return true;
}