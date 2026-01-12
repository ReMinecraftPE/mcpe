#include "ControllerBuildInput.hpp"
#include "GameControllerManager.hpp"
#include "common/Utils.hpp"

ControllerBuildInput::ControllerBuildInput()
{
    m_time = 0;
    for (int i = 0; i < 2; i++)
        m_lastButtonStates[i] = false;
}

bool ControllerBuildInput::isPerformingUse() const
{
	return GameControllerManager::getPressure(1) >= 0.75;
}

bool ControllerBuildInput::isPerformingRemove() const
{
	return GameControllerManager::getPressure(2) >= 0.75;
}

bool ControllerBuildInput::tickBuild(Player* player, BuildActionIntention* buildActionIntention)
{
    bool wroteIntention = false;
    BuildActionIntention::BuildActionIntent intent = BuildActionIntention::NONE;

    int time = getTimeMs();
    if (isPerformingUse())
    {
        wroteIntention = true;
        intent = BuildActionIntention::KEY_USE;
    }
    else if (isPerformingRemove())
    {
        if (!m_lastButtonStates[1])
            intent = BuildActionIntention::KEY_DESTROY;
        else
            intent = BuildActionIntention::DESTROY_CONTINUE;
        wroteIntention = true;
    }

    if (!wroteIntention)
    {
        if ((!isPerformingRemove() && m_lastButtonStates[1]) ||
            (!isPerformingUse() && m_lastButtonStates[0]))
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

    // Log last button states
    m_lastButtonStates[0] = isPerformingUse();
    m_lastButtonStates[1] = isPerformingRemove();

    return wroteIntention;
}
