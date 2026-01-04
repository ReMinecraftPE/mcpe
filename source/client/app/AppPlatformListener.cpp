#include "AppPlatformListener.hpp"

AppPlatformListener::AppPlatformListener(bool doInit)
{
    m_pPlatform = nullptr;

    if (doInit) initListener();
}

AppPlatformListener::~AppPlatformListener()
{
}

void AppPlatformListener::initListener(float priority)
{
    m_pPlatform = AppPlatform::singleton();
	assert(m_pPlatform);
    m_pPlatform->m_listeners.insert(AppPlatform::ListenerMap::value_type(priority, this));
}

void AppPlatformListener::terminate()
{
    onAppFocusLost();
    m_pPlatform = nullptr;
}