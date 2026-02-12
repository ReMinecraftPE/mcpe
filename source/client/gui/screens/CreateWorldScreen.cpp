/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "CreateWorldScreen.hpp"
#include "common/Util.hpp"

static char g_CreateWorldFilterArray[] = { '/','\n','\r','\x09','\0','\xC','`','?','*','\\','<','>','|','"',':' };

CreateWorldScreen::CreateWorldScreen() :
	m_textName(this, 1, 0, 0, 0, 0, "", "Unnamed world"),
	m_textSeed(this, 2, 0, 0, 0, 0, ""),
	m_btnGameMode(5, "Game Mode"),
	m_btnBack(3, "Cancel"),
	m_btnCreate(4, "Create New World")
{
	m_bDeletePrevious = false;
}

std::string CreateWorldScreen::GetUniqueLevelName(LevelStorageSource* pSource, const std::string& in)
{
	std::set<std::string> maps;

	std::vector<LevelSummary> summaries;
	pSource->getLevelList(summaries);

	for (std::vector<LevelSummary>::const_iterator it = summaries.begin(); it != summaries.end(); it++)
	{
		maps.insert(it->m_fileName);
	}

	std::string out = in;
	//unsigned int generationId = 0;
	while (maps.find(out) != maps.end())
	{
		// Custom duplicate naming scheme, so the world name matches the folder name
		/*generationId++;
		out = in + "" + Util::format("(%d)", generationId);*/

		// Java/PE default
		out += "-";
	}

	for (size_t i = 0; i < sizeof(g_CreateWorldFilterArray); i++)
	{
		std::string str;
		str.push_back(g_CreateWorldFilterArray[i]);
		Util::stringReplace(out, str, "");
	}

	return out;
}

void CreateWorldScreen::_buttonClicked(Button* pButton)
{
	if (pButton->getId() == m_btnBack.getId())
	{
		handleBackEvent(false);
	}

	if (pButton->getId() == m_btnCreate.getId())
	{
		std::string nameStr = m_textName.getText();
		std::string seedStr = m_textSeed.getText();

		std::string levelNickname = Util::stringTrim(nameStr);
		std::string levelUniqueName = GetUniqueLevelName(m_pMinecraft->m_pLevelStorageSource, levelNickname);

		int seed = int(getEpochTimeS());

		std::string seedThing = Util::stringTrim(seedStr);
		if (!seedThing.empty())
		{
			int num;
			if (sscanf(seedThing.c_str(), "%d", &num) > 0)
				seed = num;
			else
				seed = Util::hashCode(seedThing);
		}

		LevelSettings levelSettings(seed, m_gameMode);
		m_pMinecraft->selectLevel(levelUniqueName, levelNickname, levelSettings);
	}

	if (pButton->getId() == m_btnGameMode.getId())
		m_gameMode = static_cast<GameType>((static_cast<int>(m_gameMode) + 1) % (GAME_TYPES_MAX + 1));
}

#define CRAMPED() (100 + 32 + 58 + 16 > m_height)

void CreateWorldScreen::init()
{
	bool touchscreen = m_pMinecraft->isTouchscreen();

	m_textName.m_height = m_textSeed.m_height = 20;

	// provide thicker buttons for pressing on touchscreen and match the rest of the ui
	if (!touchscreen)
		m_btnBack.m_height = m_btnCreate.m_height = m_btnGameMode.m_height = m_textName.m_height = m_textSeed.m_height = 20;

	m_textName.m_width  = m_textSeed.m_width  = 200;
	m_textName.m_xPos   = m_textSeed.m_xPos   = (m_width - 200) / 2;
	m_textName.m_yPos = 60;
	m_textSeed.m_yPos = 100;
	m_btnGameMode.m_yPos  	= 140;
	m_btnGameMode.m_width   = 150;
	m_btnCreate.m_yPos  = m_height - 56;
	m_btnCreate.m_yPos 	= m_btnBack.m_yPos 	  = m_height - 30 + ((touchscreen) ? 2 : 0); // on touchscreens, align the create/cancel buttons with previous screen's Y values
	m_btnBack.m_xPos 	= (m_width / 2) + 5;
	m_btnBack.m_width   = m_btnCreate.m_width = 150;

	_addElement(m_textName);
	_addElement(m_textSeed);
	_addElement(m_btnGameMode);
	_addElement(m_btnCreate);
	_addElement(m_btnBack);
	m_textName.init(m_pFont);
	m_textSeed.init(m_pFont);

	// 100 - yPos of textSeed
	// 32  - offset + height of "Leave blank for random" text
	// 58  - approximately the Y position of the create button
	// 16  - extra offset for gamemode text to make sure it never overlaps with create new world / cancel
	bool crampedMode = CRAMPED();
	if (crampedMode)
	{
		// crush everything down as much as we can
		m_textName.m_yPos = 34;
		m_textSeed.m_yPos = 69;
		m_btnGameMode.m_yPos = m_textSeed.m_yPos + 24;
		m_btnCreate.m_width -= 50;
		m_btnBack.m_width -= 50;
	}

	// calculate after crush
	m_btnGameMode.m_xPos = m_width / 2 - m_btnGameMode.m_width / 2;
	m_btnCreate.m_xPos = m_width / 2 - m_btnCreate.m_width - 5;

	m_gameMode = GAME_TYPE_SURVIVAL;
}

void CreateWorldScreen::render(float f)
{
	renderBackground();
	Screen::render(f);

	const std::string gameTypeStr = GameTypeConv::GameTypeDescriptionToNonLocString(m_gameMode);
	m_btnGameMode.setMessage("Game Mode: " + GameTypeConv::GameTypeToNonLocString(m_gameMode));

	drawCenteredString(*m_pFont, "Create New World", m_width / 2, CRAMPED() ? 10 : 30, 0xFFFFFF);
	drawString(*m_pFont, "World name",                    m_textName.m_xPos, m_textName.m_yPos - 10, 0xDDDDDD);
	drawString(*m_pFont, "Seed for the World Generator",  m_textSeed.m_xPos, m_textSeed.m_yPos - 10, 0xDDDDDD);
	
	bool crampedMode = CRAMPED();

	// Extra text not necessarily needed
	if (crampedMode)
		return;
	
	drawString(*m_pFont, "Leave blank for a random seed", m_textSeed.m_xPos, m_textSeed.m_yPos + 22, 0x999999);
	int textCenteredX = m_btnGameMode.m_xPos + (m_btnGameMode.m_width / 2) - (m_pFont->width(gameTypeStr.c_str()) / 2);
	drawString(*m_pFont, gameTypeStr.c_str(), textCenteredX, m_btnGameMode.m_yPos + m_btnGameMode.m_height + 3, 0x999999);
}

bool CreateWorldScreen::handleBackEvent(bool b)
{
	if (!b)
	{
		m_pMinecraft->setScreen(m_pParent);
	}

	return true;
}