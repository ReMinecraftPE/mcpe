/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "KeyboardInput.hpp"
#include "world/entity/Player.hpp"

KeyboardInput::KeyboardInput(Options* pOpts)
{
	m_horzInput = 0.0f;
	m_vertInput = 0.0f;
	m_bWasJumping = false;
	m_bJumping = false;
	m_bSneaking = false;

	for (int i = 0; i < 10; i++)
		m_keys[i] = false;

	m_pOptions = pOpts;
}

void KeyboardInput::releaseAllKeys()
{
	for (int i = 0; i < 10; i++)
		m_keys[i] = false;
}

void KeyboardInput::setKey(eButtonMappingIndex ctrl, bool eventKeyState)
{
	int index;
	switch (ctrl)
	{
	case BM_FORWARD:	index = INPUT_FORWARD;	break;
	case BM_BACKWARD:	index = INPUT_BACKWARD; break;
	case BM_LEFT:		index = INPUT_LEFT;		break;
	case BM_RIGHT:		index = INPUT_RIGHT;	break;
	case BM_JUMP:		index = INPUT_JUMP;		break;
	case BM_SNEAK:		index = INPUT_SNEAK;	break;
	default:			index = -1;				return;
	}

	m_keys[index] = eventKeyState;
}

void KeyboardInput::tick(Player* pPlayer)
{
	m_horzInput = 0.0f;
	m_vertInput = 0.0f;

	if (m_keys[INPUT_FORWARD])  m_vertInput += 1.0f;
	if (m_keys[INPUT_BACKWARD]) m_vertInput -= 1.0f;
	if (m_keys[INPUT_LEFT])     m_horzInput += 1.0f;
	if (m_keys[INPUT_RIGHT])    m_horzInput -= 1.0f;

	m_bJumping  = m_keys[INPUT_JUMP];
	m_bFlyUp = m_bJumping;
	m_bSneaking = m_keys[INPUT_SNEAK];

	IMoveInput::tick(pPlayer);
}
