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
	ControllerTurnInput();
	TurnDelta getTurnDelta();

private:
	int field_8;
	int m_stickNo;
	float field_10;
	float field_14;
	bool field_18;
};

