/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "IMoveInput.hpp"

IMoveInput::IMoveInput() :
	m_horzInput(0.0f),
	m_vertInput(0.0f),
	m_flyInput(0.0f),
	field_C(false),
	m_bJumpButton(false),
	m_bSneakButton(false)
{
}

IMoveInput::~IMoveInput()
{
}

void IMoveInput::releaseAllKeys()
{
}

void IMoveInput::render(float f)
{
}

void IMoveInput::setKey(int key, bool state)
{
}

void IMoveInput::setScreenSize(int width, int height)
{
}

void IMoveInput::tick(Player* pPlayer)
{
}
