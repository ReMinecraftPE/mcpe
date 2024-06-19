/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "IMoveInput.hpp"

#include "client/options/Options.hpp"

class KeyboardInput : public IMoveInput
{
public:

public:
	KeyboardInput(Options*);

	void releaseAllKeys() override;
	void setKey(int keyCode, bool state) override;
	void tick(Player*) override;

public:
	bool m_keys[10];
	Options* m_pOptions;
};

