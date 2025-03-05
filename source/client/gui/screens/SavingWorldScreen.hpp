/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"

#ifdef ENH_IMPROVED_SAVING

class Entity;

class SavingWorldScreen : public Screen
{
public:
	SavingWorldScreen(bool bCopyMap/*, Entity* pEntityToDeleteAfterSave*/);
	
	void render(int mouseX, int mouseY, float f) override;
	void tick() override;

public:
	bool m_bCopyMapAtEnd;
	int m_timer;
	//Entity* m_pEntityToDeleteAfterSave;
};

#endif
