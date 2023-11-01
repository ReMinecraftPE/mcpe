/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "IMoveInput.hpp"
#include "ITurnInput.hpp"
#include "IBuildInput.hpp"

class IInputHolder
{
public:
	IInputHolder();
	virtual ~IInputHolder();
	virtual bool allowPicking();
	virtual void setScreenSize(int width, int height);
	virtual IMoveInput* getMoveInput() = 0;
	virtual ITurnInput* getTurnInput() = 0;
	virtual IBuildInput* getBuildInput() = 0;

protected:
	friend class UnifiedTurnBuild;
	friend class GameRenderer;
	friend class Gui;
	float m_feedbackX;
	float m_feedbackY;
	float m_feedbackAlpha;
};

