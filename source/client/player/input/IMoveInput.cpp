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
	m_bWasJumping(false),
	m_bJumping(false),
	m_bSneaking(false)
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

void IMoveInput::setKey(int eventKey, bool eventKeyState)
{
}

void IMoveInput::setScreenSize(int width, int height)
{
}

void IMoveInput::tick(Player* pPlayer)
{
	if (m_bSneaking)
	{
		m_horzInput = m_horzInput * 0.3f;
		m_vertInput = m_vertInput * 0.3f;
	}
}
