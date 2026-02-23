#pragma once

#include <string>
#include <vector>
#include "common/Random.hpp"

class SplashManager
{
public:
    void init(const std::string&);
    const std::string& getSplash();

    static SplashManager& singleton()
    {
        if (!instance)
        {
            instance = new SplashManager;
        }
        return *instance;
    }

private:
    Random m_random;
    std::vector<std::string> m_splashes;
    std::string m_isYouSplash;
    static SplashManager* instance;
};