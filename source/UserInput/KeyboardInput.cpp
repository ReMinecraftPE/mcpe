/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "KeyboardInput.hpp"

KeyboardInput::KeyboardInput(Options* pOpts)
{
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

	if (m_pOptions->m_keyBinds[0].value == keyCode) index = 0;
	if (m_pOptions->m_keyBinds[2].value == keyCode) index = 1;
	if (m_pOptions->m_keyBinds[1].value == keyCode) index = 2;
	if (m_pOptions->m_keyBinds[3].value == keyCode) index = 3;
	if (m_pOptions->m_keyBinds[4].value == keyCode) index = 4;
	if (m_pOptions->m_keyBinds[9].value == keyCode) index = 5;

	if (index == -1)
		return;

	m_keys[index] = b;
}

void KeyboardInput::tick(/* Player* */)
{
	m_horzInput = 0.0f;
	m_vertInput = 0.0f;

	if (m_keys[FORWARD])  m_vertInput += 1.0f;
	if (m_keys[BACKWARD]) m_vertInput -= 1.0f;
	if (m_keys[LEFT])     m_horzInput += 1.0f;
	if (m_keys[RIGHT])    m_horzInput -= 1.0f;

	m_bJumpButton  = m_keys[JUMP];
	m_bSneakButton = m_keys[SNEAK];

	if (m_keys[SNEAK])
	{
		m_horzInput = m_horzInput * 0.3f;
		m_vertInput = m_vertInput * 0.3f;
	}
}
