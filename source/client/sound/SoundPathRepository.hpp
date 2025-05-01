#pragma once

#include <string>
#include <vector>
#include <map>

class SoundPathRepository
{
private:
    std::vector<std::string> _all;

public:
    void add(const std::string& name, const std::string& path);
    bool get(const std::string& name, std::string& path);
    bool any(std::string& path);

public:
    std::map<std::string, std::vector<std::string> > m_repo;
};

