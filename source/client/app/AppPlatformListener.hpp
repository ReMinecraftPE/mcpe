#pragma once

#include "AppPlatform.hpp"

class AppPlatformListener
{
public:
    AppPlatformListener(bool doInit = true);
    virtual ~AppPlatformListener();

    virtual void onLowMemory() {}
    virtual void onAppResumed() {}
    virtual void onAppFocusLost() {}
    virtual void onAppFocusGained() {}
    virtual void onAppTerminated() {}
    virtual void onAppSuspended() {}

    void initListener(float priority = 1.0f);
    void terminate();

private:
    AppPlatform* m_pPlatform;
};
