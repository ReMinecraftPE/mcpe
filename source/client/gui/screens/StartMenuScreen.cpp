/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "StartMenuScreen.hpp"
#include "InvalidLicenseScreen.hpp"
#include "OptionsScreen.hpp"
#include "ProgressScreen.hpp"
#include "SelectWorldScreen.hpp"
#include "JoinGameScreen.hpp"

#ifdef _WIN32
#define CAN_QUIT
#endif

StartMenuScreen::StartMenuScreen() :
	m_startButton  (2,   0, 0, 160, 24, "Start Game"),
	m_joinButton   (3,   0, 0, 160, 24, "Join Game"),
	m_optionsButton(4,   0, 0,  78, 22, "Options"),
	m_testButton   (999, 0, 0,  78, 22, "Test"),
	m_buyButton    (5,   0, 0,  78, 22, "Buy")
	//, m_testBox(1, 10, 10, 200, 16, "Insert some text...")
{
}

void StartMenuScreen::_updateLicense()
{
	int licenseID = m_pMinecraft->getLicenseId();
	if (licenseID < 0)
	{
		m_optionsButton.m_bEnabled = false;
		m_startButton.m_bEnabled = false;
		m_joinButton.m_bEnabled = false;
	}
	else if (licenseID <= 1)
	{
		m_optionsButton.m_bEnabled = true;
		m_startButton.m_bEnabled = true;
#ifdef __EMSCRIPTEN__
		m_joinButton.m_bEnabled = false;
#else
		m_joinButton.m_bEnabled = true;
#endif
	}
	else
	{
		m_pMinecraft->setScreen(new InvalidLicenseScreen(licenseID, m_pMinecraft->platform()->hasBuyButtonWhenInvalidLicense()));
	}
}

void StartMenuScreen::buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == m_startButton.m_buttonId)
	{
#if defined(DEMO)
		m_pMinecraft->selectLevel("_DemoLevel", "_DemoLevel", int(getEpochTimeS()));
		m_pMinecraft->hostMultiplayer();
		m_pMinecraft->setScreen(new ProgressScreen);
#else
		m_pMinecraft->setScreen(new SelectWorldScreen);
#endif
	}
	else if (pButton->m_buttonId == m_joinButton.m_buttonId)
	{
		m_pMinecraft->locateMultiplayer();
		m_pMinecraft->setScreen(new JoinGameScreen);
	}
	else if (pButton->m_buttonId == m_buyButton.m_buttonId)
	{
#if !defined(DEMO) && defined(CAN_QUIT)
		m_pMinecraft->quit();
#else
		m_pMinecraft->platform()->buyGame();
#endif
	}
	else if (pButton->m_buttonId == m_optionsButton.m_buttonId)
	{
		m_pMinecraft->setScreen(new OptionsScreen);
	}
}

void StartMenuScreen::init()
{
	int yPos = m_height / 2;

	m_joinButton.m_yPos = yPos + 25;
	m_startButton.m_yPos = yPos - 3;

	yPos += 55;

	m_optionsButton.m_yPos = yPos;
	m_testButton.m_yPos = yPos;
	m_buyButton.m_yPos = yPos;

	m_startButton.m_xPos = (m_width - m_startButton.m_width) / 2;

	int x1 = m_width - m_joinButton.m_width;

	m_joinButton.m_xPos = x1 / 2;
	m_optionsButton.m_xPos = x1 / 2;
	m_buyButton.m_xPos = x1 / 2 + m_optionsButton.m_width + 4;
	m_testButton.m_xPos = x1 / 2 + m_optionsButton.m_width + 4;

	// add the buttons to the screen:
	m_buttons.push_back(&m_startButton);
	m_buttonTabList.push_back(&m_startButton);
	m_buttons.push_back(&m_joinButton);
	m_buttonTabList.push_back(&m_joinButton);
	m_buttons.push_back(&m_optionsButton);
	m_buttonTabList.push_back(&m_optionsButton);

#if defined(DEMO) || defined(CAN_QUIT)
	m_buttons.push_back(&m_buyButton);
	m_buttonTabList.push_back(&m_buyButton);
#endif

	field_154 = "\xFFMojang AB";
	field_16C = m_width - 1 - m_pFont->width(field_154);

	field_170 = "v0.1.0 alpha"
#ifdef DEMO
		" (Demo)"
#endif
		;

	field_188 = (m_width - m_pFont->width(field_170)) / 2;

#if !defined(DEMO) && defined(CAN_QUIT)
	m_buyButton.m_text = "Quit";
#endif

	//m_testBox.init(m_pFont);
	//m_textInputs.push_back(&m_testBox);

	_updateLicense();
}

bool StartMenuScreen::isInGameScreen()
{
	return false;
}

void StartMenuScreen::render(int a, int b, float c)
{
	renderBackground();

	Textures* tx = m_pMinecraft->m_pTextures;

	int id = tx->loadTexture("gui/title.png", true);
	Texture *pTex = tx->getTemporaryTextureData(id);

	if (pTex)
	{
		if (id != tx->m_currBoundTex)
		{
			glBindTexture(GL_TEXTURE_2D, id);
			tx->m_currBoundTex = id;
		}

		int left = (m_width - pTex->m_width) / 2;
		int width  = pTex->m_width;
		int height = pTex->m_height;

		Tesselator& t = Tesselator::instance;
		glColor4f(1, 1, 1, 1);
		t.begin();
		t.vertexUV(float(left),         float(height + 4), field_4, 0.0f, 1.0f);
		t.vertexUV(float(left + width), float(height + 4), field_4, 1.0f, 1.0f);
		t.vertexUV(float(left + width),                4,  field_4, 1.0f, 0.0f);
		t.vertexUV(float(left),                        4,  field_4, 0.0f, 0.0f);
		t.draw();
	}

	drawString(m_pFont, field_170, field_188, 62, 0xFFCCCCCC);
	drawString(m_pFont, field_154, field_16C, m_height - 10, 0x00FFFFFF);

	Screen::render(a, b, c);
}

void StartMenuScreen::tick()
{
	_updateLicense();
}
