/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "ITurnInput.hpp"
#include "Minecraft.hpp"
class Minecraft;

class MouseTurnInput : public ITurnInput
{
public:
	MouseTurnInput(Minecraft*);
	Delta getTurnDelta() override;

private:
	int m_lastX = -1;
	int m_lastY = -1;

	Minecraft* m_pMinecraft;
};

