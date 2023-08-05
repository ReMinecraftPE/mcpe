/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "RenameMPLevelScreen.hpp"
#include "StartMenuScreen.hpp"

RenameMPLevelScreen::RenameMPLevelScreen(const std::string& levelName) : m_levelName(levelName)
{
}

void RenameMPLevelScreen::init()
{
	m_pMinecraft->platform()->showDialog(AppPlatform::DLG_RENAME_MP_WORLD);
	m_pMinecraft->platform()->createUserInput();
}

void RenameMPLevelScreen::render(int mouseX, int mouseY, float f)
{
	int userInputStatus = m_pMinecraft->platform()->getUserInputStatus();
	if (userInputStatus < 0)
		return;

	if (userInputStatus == 1)
	{
		std::vector<std::string> input = m_pMinecraft->platform()->getUserInput();
		if (input.size() > 0 && !input[0].empty())
		{
			m_pMinecraft->getLevelSource()->renameLevel(m_levelName, input[0]);
		}
	}

	m_pMinecraft->setScreen(new StartMenuScreen);
}
