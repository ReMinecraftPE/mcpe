/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SelectWorldScreen.hpp"
#include "DeleteWorldScreen.hpp"
#include "CreateWorldScreen.hpp"
#include "ProgressScreen.hpp"
#include "StartMenuScreen.hpp"
#include "client/common/Util.hpp"

SelectWorldScreen::SelectWorldScreen() :
	m_btnDelete   (1, "Delete"),
	m_btnCreateNew(2, "Create new"),
	m_btnBack     (3, "Back"),
	m_btnUnknown  (4, "")
{
	m_btnDelete.m_bEnabled = false;
}

void SelectWorldScreen::init()
{
	m_pWorldSelectionList = new WorldSelectionList(m_pMinecraft, m_width, m_height);
	loadLevelSource();
	m_pWorldSelectionList->commit();

	m_btnDelete.m_yPos   = m_btnBack.m_yPos   = m_btnCreateNew.m_yPos    = m_height - 28;
	m_btnDelete.m_width  = m_btnBack.m_width  = m_btnCreateNew.m_width  = 84;
	m_btnDelete.m_height = m_btnBack.m_height = m_btnCreateNew.m_height = 24;

	m_btnDelete.m_xPos    = m_width / 2 - 130;
	m_btnCreateNew.m_xPos = m_width / 2 - 42;
	m_btnBack.m_xPos      = m_width / 2 + 46;

	m_buttons.push_back(&m_btnCreateNew);
	m_buttons.push_back(&m_btnBack);
	m_buttons.push_back(&m_btnDelete);

	field_12C = Mouse::_buttonStates[1] == 0;

	m_buttonTabList.push_back(&m_btnUnknown);
	m_buttonTabList.push_back(&m_btnDelete);
	m_buttonTabList.push_back(&m_btnCreateNew);
	m_buttonTabList.push_back(&m_btnBack);
}

bool SelectWorldScreen::isInGameScreen()
{
	return true;
}

void SelectWorldScreen::keyPressed(int code)
{
#ifndef ORIGINAL_CODE
	if (m_pMinecraft->m_options.m_keyBinds[Options::MENU_OK].value == code)
		m_pWorldSelectionList->selectItem(m_pWorldSelectionList->getItemAtPosition(m_width / 2, m_height / 2), false);

	m_btnUnknown.field_36 = true;
#endif

	if (m_btnUnknown.field_36)
	{
		if (m_pMinecraft->m_options.m_keyBinds[Options::LEFT].value == code)
			m_pWorldSelectionList->stepLeft();

		if (m_pMinecraft->m_options.m_keyBinds[Options::RIGHT].value == code)
			m_pWorldSelectionList->stepRight();
	}

	Screen::keyPressed(code);
}

static char g_SelectWorldFilterArray[] = { '/','\n','\r','\x09','\0','\xC','`','?','*','\\','<','>','|','"',':'};

void SelectWorldScreen::tick()
{
#ifndef ORIGINAL_CODE
	m_btnUnknown.field_36 = true;
#endif

	if (field_130 == 1)
	{
		// poll the user status to get details about the world name and seed
		int userInputStatus = m_pMinecraft->platform()->getUserInputStatus();

		if (userInputStatus < 0)
			return;

		if (userInputStatus != 1)
		{
			field_130 = 0;
			return;
		}

		std::vector<std::string> userInput = m_pMinecraft->platform()->getUserInput();

		std::string levelNickname = Util::stringTrim(userInput[0]);
		std::string levelUniqueName = levelNickname;

		for (int i = 0; i < sizeof(g_SelectWorldFilterArray); i++)
		{
			std::string str;
			str.push_back(g_SelectWorldFilterArray[i]);
			Util::stringReplace(levelUniqueName, str, "");
		}

		levelUniqueName = getUniqueLevelName(levelUniqueName);

		int seed = int(getEpochTimeS());
		if (userInput.size() > 1)
		{
			std::string seedThing = Util::stringTrim(userInput[1]);
			if (!seedThing.empty())
			{
				int num;
				if (sscanf(seedThing.c_str(), "%d", &num) > 0)
					seed = num;
				else
					seed = Util::hashCode(seedThing);
			}
		}

		m_pMinecraft->selectLevel(levelUniqueName, levelNickname, seed);
		m_pMinecraft->hostMultiplayer();
		m_pMinecraft->setScreen(new ProgressScreen);

		// @BUG: Use of deallocated memory. SetScreen frees us
#ifdef ORIGINAL_CODE
		field_130 = 0;
#endif

		return;
	}

	m_pWorldSelectionList->tick();
	if (m_pWorldSelectionList->field_90)
	{
		LevelSummary& ls = m_pWorldSelectionList->m_levelSummary;
		m_pMinecraft->selectLevel(ls.field_0, ls.field_18, 0);
		m_pMinecraft->hostMultiplayer();
		m_pMinecraft->setScreen(new ProgressScreen);
		return;
	}

	// the level summary stuff is unused.
	LevelSummary ls;
	if (isIndexValid(m_pWorldSelectionList->m_selectedIndex))
		ls = m_pWorldSelectionList->m_items[m_pWorldSelectionList->m_selectedIndex];

	m_btnDelete.m_bEnabled = isIndexValid(m_pWorldSelectionList->m_selectedIndex);
}

void SelectWorldScreen::render(int mouseX, int mouseY, float f)
{
	renderBackground();
#ifndef ORIGINAL_CODE
	m_btnUnknown.field_36 = true;
#endif
	m_pWorldSelectionList->setComponentSelected(m_btnUnknown.field_36);
	if (field_12C)
	{
		m_pWorldSelectionList->render(mouseX, mouseY, f);
	}
	else
	{
		m_pWorldSelectionList->render(0, 0, f);
		field_12C = Mouse::_buttonStates[1] == 0;
	}

	Screen::render(mouseX, mouseY, f);

	drawCenteredString(m_pMinecraft->m_pFont, "Select world", m_width / 2, 8, 0xFFFFFFFF);
}

bool SelectWorldScreen::handleBackEvent(bool b)
{
	if (b)
		return true;

	// @TODO: m_pMinecraft->cancelLocateMultiplayer();
	m_pMinecraft->setScreen(new StartMenuScreen);
	return true;
}

void SelectWorldScreen::buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == m_btnCreateNew.m_buttonId)
	{
#ifndef ORIGINAL_CODE
		m_pMinecraft->setScreen(new CreateWorldScreen);
#else
		m_pMinecraft->platform()->showDialog(AppPlatform::DLG_CREATE_WORLD);
		m_pMinecraft->platform()->createUserInput();
		field_130 = true;
#endif
	}

	if (pButton->m_buttonId == m_btnDelete.m_buttonId)
	{
		LevelSummary ls(m_pWorldSelectionList->m_items[m_pWorldSelectionList->m_selectedIndex]);
		m_pMinecraft->setScreen(new DeleteWorldScreen(ls));
	}

	if (pButton->m_buttonId == m_btnBack.m_buttonId)
	{
		// @TODO: m_pMinecraft->cancelLocateMultiplayer();
		m_pMinecraft->setScreen(new StartMenuScreen);
	}

	if (pButton->m_buttonId == m_btnUnknown.m_buttonId)
	{
		m_pWorldSelectionList->selectItem(m_pWorldSelectionList->getItemAtPosition(m_width / 2, m_height / 2), false);
	}
}

bool SelectWorldScreen::isIndexValid(int idx)
{
	if (!m_pWorldSelectionList)
		return false;

	if (idx < 0)
		return false;

	if (idx >= m_pWorldSelectionList->getNumberOfItems())
		return false;

	return true;
}

std::string SelectWorldScreen::getUniqueLevelName(const std::string& in)
{
	std::set<std::string> maps;

	for (const auto& ls : m_levels)
	{
		maps.insert(ls.field_0);
	}

	std::string out = in;
	while (maps.find(out) != maps.end())
		out += "-";

	return out;
}

void SelectWorldScreen::loadLevelSource()
{
	m_pMinecraft->getLevelSource()->getLevelList(m_levels);

	std::sort(m_levels.begin(), m_levels.end());

	for (const auto& level : m_levels)
	{
		if (level.field_0 == "_LastJoinedServer")
			continue;

		m_pWorldSelectionList->m_items.push_back(level);
	}
}
