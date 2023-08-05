/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

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

