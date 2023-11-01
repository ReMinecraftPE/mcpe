#pragma once

#include "ITurnInput.hpp"

class MouseHandler
{
public:
	MouseHandler();
	void setTurnInput(ITurnInput*);
	void grab();
	void poll();
	void release();
	bool smoothTurning();

public:
	TurnDelta m_delta;
	ITurnInput* m_pTurnInput;
};

