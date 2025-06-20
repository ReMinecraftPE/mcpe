#pragma once

#include "IBuildInput.hpp"
#include "MouseDevice.hpp"

class MouseBuildInput : public IBuildInput
{
private:
	unsigned int m_time;
	bool m_lastButtonStates[BUTTON_COUNT];

public:
	MouseBuildInput();

private:
	void _updateLastButtonStates();

public:
	virtual bool tickBuild(Player* player, BuildActionIntention* buildActionIntention) override;
};

