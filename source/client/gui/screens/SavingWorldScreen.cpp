/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SavingWorldScreen.hpp"
#include "RenameMPLevelScreen.hpp"
#include "StartMenuScreen.hpp"

#ifdef ENH_IMPROVED_SAVING

SavingWorldScreen::SavingWorldScreen(bool bCopyMap)
{
	m_bCopyMapAtEnd = bCopyMap;
	m_timer = 0;
}

void SavingWorldScreen::render(int mouseX, int mouseY, float f)
{
	renderDirtBackground(0);

	int x_width  = int(Minecraft::width  * Gui::InvGuiScale);
	int x_height = int(Minecraft::height * Gui::InvGuiScale);
	int yPos = x_height / 2;

	int width = m_pFont->width("Saving chunks");
	m_pFont->drawShadow("Saving chunks", (x_width - width) / 2, yPos, 0xFFFFFF);
}

void SavingWorldScreen::tick()
{
	if (m_timer < 0)
		return;

	m_timer++;

	if (m_timer >= 5)
	{
		m_timer = -1;

		Level* pLevel = m_pMinecraft->m_pLevel;
		if (pLevel)
		{
			pLevel->saveUnsavedChunks();
			pLevel->saveLevelData();
			pLevel->savePlayerData();

			LevelStorage* pStorage = pLevel->getLevelStorage();
			SAFE_DELETE(pStorage);
			SAFE_DELETE(pLevel);

			m_pMinecraft->m_pLevel = nullptr;
		}

		m_pMinecraft->m_bUsingScreen = true;

		if (m_bCopyMapAtEnd)
			m_pMinecraft->setScreen(new RenameMPLevelScreen("_LastJoinedServer"));
		else
			m_pMinecraft->setScreen(new StartMenuScreen);

		m_pMinecraft->m_bUsingScreen = false;

		m_pMinecraft->field_288 = false;
	}
}

#endif
