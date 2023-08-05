/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "DeleteWorldScreen.hpp"
#include "SelectWorldScreen.hpp"

DeleteWorldScreen::DeleteWorldScreen(const LevelSummary& level) :
	ConfirmScreen(nullptr,
		"Are you sure you want to delete this world?",
		"'" + level.field_18 + "' will be lost forever!",
		"Delete", "Cancel", 0),
	m_level(level)
{
	// highlight the cancel button so the user will have to do 1 extra action to delete their world
	m_tabButtonIndex = 1;
}

void DeleteWorldScreen::postResult(bool b)
{
	if (b)
	{
		m_pMinecraft->getLevelSource()->deleteLevel(m_level.field_0);
	}

	m_pMinecraft->setScreen(new SelectWorldScreen);
}
