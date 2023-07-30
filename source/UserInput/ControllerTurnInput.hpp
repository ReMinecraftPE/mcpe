#pragma once

#include "ITurnInput.hpp"

class ControllerTurnInput : public ITurnInput
{
public:
	ITurnInput::Delta getTurnDelta();

private:
	int field_8 = 2;
	int m_stickNo = 2;
	float field_10 = 0.0f;
	float field_14 = 0.0f;
	bool field_18 = false;
};

