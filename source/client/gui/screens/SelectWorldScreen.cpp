/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SelectWorldScreen.hpp"
#include "DeleteWorldScreen.hpp"
#include "CreateWorldScreen.hpp"
#include "StartMenuScreen.hpp"
#include "ProgressScreen.hpp"
#include "common/Util.hpp"

SelectWorldScreen::SelectWorldScreen() :
	m_btnDelete   ("Delete"),
	m_btnCreateNew("Create New"),
	m_btnBack     ("Back"),
	m_btnWorld    (""),
	m_pWorldSelectionList(nullptr)
{
	m_btnDelete.setEnabled(false);
	m_btnWorld.m_color.a = 0.0f;
	field_12C = false;
	field_130 = 0;
	m_pSelectedElement = &m_btnWorld;
}

SelectWorldScreen::~SelectWorldScreen()
{
    SAFE_DELETE(m_pWorldSelectionList);
}

bool SelectWorldScreen::_areaNavigation(AreaNavigation::Direction dir)
{
	if (m_btnWorld.isSelected() && (dir == AreaNavigation::LEFT || dir == AreaNavigation::RIGHT))
	{
		if (dir == AreaNavigation::LEFT)
			m_pWorldSelectionList->stepLeft();
		else
			m_pWorldSelectionList->stepRight();

		return true;
	}

	return Screen::_areaNavigation(dir);
}

void SelectWorldScreen::init()
{
	SAFE_DELETE(m_pWorldSelectionList);

	m_pWorldSelectionList = new WorldSelectionList(m_pMinecraft, m_width, m_height);
	loadLevelSource();
	m_pWorldSelectionList->commit();

	m_btnDelete.m_yPos   = m_btnBack.m_yPos   = m_btnCreateNew.m_yPos   = m_height - 28;
	m_btnDelete.m_width  = m_btnBack.m_width  = m_btnCreateNew.m_width  = 84;
	m_btnDelete.m_height = m_btnBack.m_height = m_btnCreateNew.m_height = 24;

	m_btnDelete.m_xPos    = m_width / 2 - 130;
	m_btnCreateNew.m_xPos = m_width / 2 - 42;
	m_btnBack.m_xPos      = m_width / 2 + 46;

	_addElement(m_btnWorld);
	_addElement(m_btnDelete);
	_addElement(m_btnCreateNew);
	_addElement(m_btnBack);

	field_12C = m_menuPointer.isPressed;
}

bool SelectWorldScreen::isInGameScreen()
{
	return true;
}

void SelectWorldScreen::keyPressed(int code)
{
#ifndef ORIGINAL_CODE
	if (m_pMinecraft->getOptions()->getKey(KM_MENU_OK) == code)
		m_pWorldSelectionList->selectItem(m_pWorldSelectionList->getItemAtPosition(m_width / 2, m_height / 2), false);
#endif

	if (m_btnWorld.isSelected())
	{
		if (m_pMinecraft->getOptions()->getKey(KM_LEFT) == code)
			m_pWorldSelectionList->stepLeft();

		if (m_pMinecraft->getOptions()->getKey(KM_RIGHT) == code)
			m_pWorldSelectionList->stepRight();
	}

	Screen::keyPressed(code);
}

static char g_SelectWorldFilterArray[] = { '/','\n','\r','\x09','\0','\xC','`','?','*','\\','<','>','|','"',':'};

void SelectWorldScreen::tick()
{
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

		for (size_t i = 0; i < sizeof(g_SelectWorldFilterArray); i++)
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

		GameType gameType = GAME_TYPE_CREATIVE;
		if (userInput.size() > 2 && userInput[2].compare("survival") == 0)
			gameType = GAME_TYPE_SURVIVAL;

		LevelSettings settings(seed, gameType);
		m_pMinecraft->selectLevel(levelUniqueName, levelNickname, settings);

		// @BUG: Use of deallocated memory. SetScreen frees us
#ifdef ORIGINAL_CODE
		field_130 = 0;
#endif

		return;
	}

	m_pWorldSelectionList->tick();
	if (m_pWorldSelectionList->field_90)
	{
        m_pMinecraft->selectLevel(m_pWorldSelectionList->m_levelSummary);
		return;
	}

	// the level summary stuff is unused.
	/*LevelSummary ls;
	if (isIndexValid(m_pWorldSelectionList->m_selectedIndex))
		ls = m_pWorldSelectionList->m_items[m_pWorldSelectionList->m_selectedIndex];*/

	m_btnDelete.setEnabled(isIndexValid(m_pWorldSelectionList->m_selectedIndex));
}

void SelectWorldScreen::render(float f)
{
	renderBackground();

	m_pWorldSelectionList->setComponentSelected(m_btnWorld.isSelected());
	if (field_12C)
	{
		m_pWorldSelectionList->render(m_menuPointer, f);
	}
	else
	{
		MenuPointer tempPointer;
		m_pWorldSelectionList->render(tempPointer, f);
		field_12C = m_menuPointer.isPressed;
	}

	Screen::render(f);

	drawCenteredString(*m_pMinecraft->m_pFont, "Select world", m_width / 2, 8, 0xFFFFFFFF);
}

bool SelectWorldScreen::handleBackEvent(bool b)
{
	if (b)
		return true;

	// @TODO: m_pMinecraft->cancelLocateMultiplayer();
	m_pMinecraft->setScreen(new StartMenuScreen);
	return true;
}

void SelectWorldScreen::_buttonClicked(Button* pButton)
{
	if (pButton->getId() == m_btnCreateNew.getId())
	{
#ifndef ORIGINAL_CODE
		m_pMinecraft->setScreen(new CreateWorldScreen);
#else
		m_pMinecraft->platform()->showDialog(AppPlatform::DLG_CREATE_WORLD);
		m_pMinecraft->platform()->createUserInput();
		field_130 = true;
#endif
	}

	if (pButton->getId() == m_btnDelete.getId())
	{
		LevelSummary ls(m_pWorldSelectionList->m_items[m_pWorldSelectionList->m_selectedIndex]);
		m_pMinecraft->setScreen(new DeleteWorldScreen(ls));
	}

	if (pButton->getId() == m_btnBack.getId())
	{
		// @TODO: m_pMinecraft->cancelLocateMultiplayer();
		m_pMinecraft->setScreen(new StartMenuScreen);
	}

	if (pButton->getId() == m_btnWorld.getId())
	{
		m_pWorldSelectionList->selectItem(m_pWorldSelectionList->getItemAtPosition(m_width / 2, m_height / 2), false);
	}
}

void SelectWorldScreen::handleScrollWheel(float force)
{
	m_pWorldSelectionList->handleScrollWheel(force);
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

	for (size_t i = 0; i < m_levels.size(); i++)
	{
		const LevelSummary& ls = m_levels[i];
		maps.insert(ls.m_fileName);
	}

	std::string out = in;
	while (maps.find(out) != maps.end())
		out += "-";

	return out;
}

void SelectWorldScreen::loadLevelSource()
{
	m_pWorldSelectionList->m_items.clear();

	m_levels.clear();

	m_pMinecraft->getLevelSource()->getLevelList(m_levels);

	std::sort(m_levels.begin(), m_levels.end());
	
	for (size_t i = 0; i < m_levels.size(); i++)
	{
		const LevelSummary& level = m_levels[i];

		if (level.m_fileName == "_LastJoinedServer")
			continue;

		m_pWorldSelectionList->m_items.push_back(level);
	}
}
