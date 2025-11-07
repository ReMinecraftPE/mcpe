#include "AppPlatformListener.hpp"

AppPlatformListener::AppPlatformListener(bool doInit)
{
    m_pPlatform = nullptr;

    if (doInit) initListener();
}

void AppPlatformListener::initListener(float priority)
{
    m_pPlatform = AppPlatform::singleton();
    m_pPlatform->m_listeners.insert({priority, this});
}

void AppPlatformListener::terminate()
{
    onAppFocusLost();
    m_pPlatform = nullptr;
}