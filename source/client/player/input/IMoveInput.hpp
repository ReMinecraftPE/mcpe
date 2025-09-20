/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "compat/LegacyCPP.hpp"

class Player;

enum
{
	INPUT_FORWARD,
	INPUT_BACKWARD,
	INPUT_LEFT,
	INPUT_RIGHT,
	INPUT_JUMP,
	INPUT_SNEAK,
	INPUT_FORWARDLEFT,
	INPUT_FORWARDRIGHT,
};

class IMoveInput
{
public:
	IMoveInput();
	virtual ~IMoveInput();

	virtual void releaseAllKeys();
	virtual void render(float f);
	virtual void setKey(int eventKey, bool eventKeyState);
	virtual void setScreenSize(int width, int height);
	virtual void tick(Player*);

public:
	float m_horzInput;
	float m_vertInput;
	bool m_bWasJumping;
	bool m_bJumping;
	bool m_bSneaking;
};

