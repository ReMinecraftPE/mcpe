#include "MouseBuildInput.hpp"
#include "Mouse.hpp"
#include "common/Utils.hpp"

MouseBuildInput::MouseBuildInput()
{
    m_time = 0;
    for (int i = 0; i < BUTTON_COUNT; i++)
        m_lastButtonStates[i] = false;
}

void MouseBuildInput::_updateLastButtonStates()
{
    m_lastButtonStates[BUTTON_LEFT]   = Mouse::isButtonDown(BUTTON_LEFT);
    m_lastButtonStates[BUTTON_RIGHT]  = Mouse::isButtonDown(BUTTON_RIGHT);
    m_lastButtonStates[BUTTON_MIDDLE] = Mouse::isButtonDown(BUTTON_MIDDLE);
}

bool MouseBuildInput::tickBuild(Player* player, BuildActionIntention* buildActionIntention)
{
    bool wroteIntention = false;
    BuildActionIntention::BuildActionIntent intent = BuildActionIntention::NONE;

    int time = getTimeMs();
    if (Mouse::isButtonDown(BUTTON_RIGHT))
    {
        wroteIntention = true;
        intent = BuildActionIntention::KEY_USE;
    }
    else if (Mouse::isButtonDown(BUTTON_LEFT))
    {
        // Mouse controls should never be able to invoke DESTROY_START behavior
        if (!m_lastButtonStates[BUTTON_LEFT])
        {
            // Button was just clicked
            wroteIntention = true;
            intent = BuildActionIntention::KEY_DESTROY;
        }
        else
        {
            wroteIntention = true;
            intent = BuildActionIntention::DESTROY_CONTINUE; //  we don't wanna slap any mobs around
        }
    }
    else if (Mouse::isButtonDown(BUTTON_MIDDLE))
    {
        if (!m_lastButtonStates[BUTTON_MIDDLE])
        {
            wroteIntention = true;
            intent = BuildActionIntention::PICK;
        }
    }

    if (!wroteIntention)
    {
        if ((!Mouse::isButtonDown(BUTTON_LEFT) && m_lastButtonStates[BUTTON_LEFT]) ||
            (!Mouse::isButtonDown(BUTTON_RIGHT) && m_lastButtonStates[BUTTON_RIGHT]))
        {
            // A button was let go
            wroteIntention = true;
            intent = BuildActionIntention::NONE;
        }
    }

    if (wroteIntention)
    {
        m_time = time;
        *buildActionIntention = BuildActionIntention(intent);
    }

    _updateLastButtonStates();

    return wroteIntention;
}