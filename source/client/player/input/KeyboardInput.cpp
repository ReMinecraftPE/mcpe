/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "KeyboardInput.hpp"

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

void KeyboardInput::setKey(int eventKey, bool eventKeyState)
{
	int index = -1;

	if      (m_pOptions->getKey(KM_FORWARD)  == eventKey) index = INPUT_FORWARD;
	else if (m_pOptions->getKey(KM_BACKWARD) == eventKey) index = INPUT_BACKWARD;
	else if (m_pOptions->getKey(KM_LEFT)     == eventKey) index = INPUT_LEFT;
	else if (m_pOptions->getKey(KM_RIGHT)    == eventKey) index = INPUT_RIGHT;
	else if (m_pOptions->getKey(KM_JUMP)     == eventKey) index = INPUT_JUMP;
	else if (m_pOptions->getKey(KM_SNEAK)    == eventKey) index = INPUT_SNEAK;

	if (index == -1)
		return;

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
	m_bSneaking = m_keys[INPUT_SNEAK];

	IMoveInput::tick(pPlayer);
}
