#include "SimpleSoundRepository.hpp"

#include "common/Utils.hpp"
#include "common/Mth.hpp"

void SimpleSoundRepository::add(const std::string& name, const std::string& path)
{
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

bool SimpleSoundRepository::get(const std::string& name, std::string& path)
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
