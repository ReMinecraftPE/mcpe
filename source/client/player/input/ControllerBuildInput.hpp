#pragma once

#include "IBuildInput.hpp"

class ControllerBuildInput : public IBuildInput
{
private:
	unsigned int m_time;
	bool m_lastButtonStates[2];

public:
	ControllerBuildInput();

	bool isPerformingUse() const;
	bool isPerformingRemove() const;
	virtual bool tickBuild(Player* player, BuildActionIntention* buildActionIntention) override;
};

