/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "client/options/Options.hpp"

class KeyboardInput
{
public:
	enum
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		JUMP,
		SNEAK,
	};

public:
	KeyboardInput(Options*);

	virtual void releaseAllKeys();
	virtual void setKey(int index, bool b);
	virtual void tick(/* Player* */);

public:
	float m_horzInput;
	float m_vertInput ;
	bool field_C;
	bool m_bJumpButton;
	bool m_bSneakButton;
	bool m_keys[10];
	Options* m_pOptions;
};

