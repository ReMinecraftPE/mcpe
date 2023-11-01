/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "IInputHolder.hpp"
#include "TouchscreenInput_TestFps.hpp"
#include "UnifiedTurnBuild.hpp"

class Minecraft;
class Options;

class TouchInputHolder : public IInputHolder
{
public:
	TouchInputHolder(Minecraft*, Options*);
	bool allowPicking() override;
	IMoveInput* getMoveInput() override;
	ITurnInput* getTurnInput() override;
	IBuildInput* getBuildInput() override;
	void setScreenSize(int width, int height) override;

public:
	TouchscreenInput_TestFps m_touchScreenInput;
	UnifiedTurnBuild m_unifiedTurnBuild;
	Minecraft* m_pMinecraft;
};

