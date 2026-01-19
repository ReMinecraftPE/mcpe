/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "ITurnInput.hpp"
#include "world/phys/Vec2.hpp"

class ControllerTurnInput : public ITurnInput
{
public:
	ControllerTurnInput();
	Vec2 getTurnDelta() override;
	bool smoothTurning() override;

private:
	int field_8;
	int m_stickNo;
	Vec2 m_analogTurnVector;
	bool field_18;
};

