/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "CreateWorldScreen.hpp"
#include "SelectWorldScreen.hpp"
#include "ProgressScreen.hpp"
#include "client/common/Util.hpp"

CreateWorldScreen::CreateWorldScreen() :
	m_textName(1, 0, 0, 0, 0, "", "Unnamed world"),
	m_textSeed(2, 0, 0, 0, 0, ""),
	m_btnBack(3, "Cancel"),
	m_btnCreate(4, "Create New World")
{
}

void CreateWorldScreen::init()
{
	m_textName.m_width  = m_textSeed.m_width  = 200;
	m_textName.m_height = m_textSeed.m_height = 20;
	m_textName.m_xPos   = m_textSeed.m_xPos   = (m_width - 200) / 2;
	m_textName.m_yPos = 60;
	m_textSeed.m_yPos = 100;

	m_btnCreate.m_yPos  = m_height - 56;
	m_btnBack.m_yPos    = m_height - 30;
	m_btnBack.m_width   = m_btnCreate.m_width  = 200;
	m_btnBack.m_height  = m_btnCreate.m_height = 20;
	
	m_btnBack.m_xPos   = m_width / 2 - 200 / 2;
	m_btnCreate.m_xPos = m_width / 2 - 200 / 2;

	m_textInputs.push_back(&m_textName);
	m_textInputs.push_back(&m_textSeed);
	m_buttons.push_back(&m_btnBack);
	m_buttons.push_back(&m_btnCreate);
	m_buttonTabList.push_back(&m_btnBack);
	m_buttonTabList.push_back(&m_btnCreate);
	m_textName.init(m_pFont);
	m_textSeed.init(m_pFont);
}

static char g_CreateWorldFilterArray[] = { '/','\n','\r','\x09','\0','\xC','`','?','*','\\','<','>','|','"',':' };

static std::string GetUniqueLevelName(LevelStorageSource* pSource, const std::string& in)
{
	std::set<std::string> maps;

	std::vector<LevelSummary> vls;
	pSource->getLevelList(vls);

	for (const auto& ls : vls)
	{
		maps.insert(ls.field_0);
	}

	std::string out = in;
	while (maps.find(out) != maps.end())
		out += "-";

	return out;
}

void CreateWorldScreen::buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == m_btnBack.m_buttonId)
	{
		m_pMinecraft->setScreen(new SelectWorldScreen);
	}

	if (pButton->m_buttonId == m_btnCreate.m_buttonId)
	{
		std::string nameStr = m_textName.m_text;
		std::string seedStr = m_textSeed.m_text;

		std::string levelNickname = Util::stringTrim(nameStr);
		std::string levelUniqueName = levelNickname;

		for (int i = 0; i < sizeof(g_CreateWorldFilterArray); i++)
		{
			std::string str;
			str.push_back(g_CreateWorldFilterArray[i]);
			Util::stringReplace(levelUniqueName, str, "");
		}

		levelUniqueName = GetUniqueLevelName(m_pMinecraft->m_pLevelStorageSource, levelUniqueName);

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

		m_pMinecraft->selectLevel(levelUniqueName, levelNickname, seed);
		m_pMinecraft->hostMultiplayer();
		m_pMinecraft->setScreen(new ProgressScreen);
	}
}

void CreateWorldScreen::render(int mouseX, int mouseY, float f)
{
	renderBackground();
	Screen::render(mouseX, mouseY, f);

	drawCenteredString(m_pFont, "Create New World", m_width / 2, 30, 0xFFFFFF);
	drawString(m_pFont, "World name",                    m_textName.m_xPos, m_textName.m_yPos - 10, 0xDDDDDD);
	drawString(m_pFont, "Seed for the World Generator",  m_textSeed.m_xPos, m_textSeed.m_yPos - 10, 0xDDDDDD);
	drawString(m_pFont, "Leave blank for a random seed", m_textSeed.m_xPos, m_textSeed.m_yPos + 22, 0x999999);
}
