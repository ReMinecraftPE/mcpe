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
	field_C = false;
	m_bJumpButton = false;
	m_bSneakButton = false;

	for (int i = 0; i < 10; i++)
		m_keys[i] = false;

	m_pOptions = pOpts;
}

void KeyboardInput::releaseAllKeys()
{
	for (int i = 0; i < 10; i++)
		m_keys[i] = false;
}

void KeyboardInput::setKey(int keyCode, bool b)
{
	int index = -1;

	if (m_pOptions->getKey(KM_FORWARD)  == keyCode) index = 0;
	if (m_pOptions->getKey(KM_BACKWARD) == keyCode) index = 1;
	if (m_pOptions->getKey(KM_LEFT)     == keyCode) index = 2;
	if (m_pOptions->getKey(KM_RIGHT)    == keyCode) index = 3;
	if (m_pOptions->getKey(KM_JUMP)     == keyCode) index = 4;
	if (m_pOptions->getKey(KM_SNEAK)    == keyCode) index = 5;

	if (index == -1)
		return;

	m_keys[index] = b;
}

void KeyboardInput::tick(Player* pPlayer)
{
	m_horzInput = 0.0f;
	m_vertInput = 0.0f;

	if (m_keys[INPUT_FORWARD])  m_vertInput += 1.0f;
	if (m_keys[INPUT_BACKWARD]) m_vertInput -= 1.0f;
	if (m_keys[INPUT_LEFT])     m_horzInput += 1.0f;
	if (m_keys[INPUT_RIGHT])    m_horzInput -= 1.0f;

	m_bJumpButton  = m_keys[INPUT_JUMP];
	m_bSneakButton = m_keys[INPUT_SNEAK];

	if (m_keys[INPUT_SNEAK])
	{
		m_horzInput = m_horzInput * 0.3f;
		m_vertInput = m_vertInput * 0.3f;
	}
}
