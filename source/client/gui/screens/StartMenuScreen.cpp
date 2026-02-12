/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "StartMenuScreen.hpp"

#include "client/renderer/ScreenRenderer.hpp"
#include "client/renderer/LogoRenderer.hpp"

#include "InvalidLicenseScreen.hpp"
#include "SelectWorldScreen.hpp"
#include "JoinGameScreen.hpp"
#include "client/resources/SplashManager.hpp"

StartMenuScreen::StartMenuScreen() :
	m_startButton  (0, 0, 160, 24, "Start Game"),
	m_joinButton   (0, 0, 160, 24, "Join Game"),
	m_optionsButton(0, 0,  78, 22, "Options"),
	m_buyButton    (0, 0,  78, 22, "Buy"),
	m_creditsButton(0, 0,  78, 22, "")
{
	m_chosenSplash = SplashManager::singleton().getSplash();
}

StartMenuScreen::~StartMenuScreen()
{
}

void StartMenuScreen::_updateLicense()
{
	int licenseID = m_pMinecraft->getLicenseId();
	if (licenseID < 0)
	{
		m_optionsButton.setEnabled(false);
		m_startButton.setEnabled(false);
		m_joinButton.setEnabled(false);
	}
	else if (licenseID <= 1)
	{
		m_optionsButton.setEnabled(true);
		m_startButton.setEnabled(true);
#ifdef FEATURE_NETWORKING
		m_joinButton.setEnabled(true);
#else
		m_joinButton.setEnabled(false);
#endif
	}
	else
	{
		m_pMinecraft->setScreen(new InvalidLicenseScreen(licenseID, m_pMinecraft->platform()->hasBuyButtonWhenInvalidLicense()));
	}
}

void StartMenuScreen::_buttonClicked(Button* pButton)
{
	if (pButton->getId() == m_startButton.getId())
	{
#if defined(DEMO)
		m_pMinecraft->selectLevel("_DemoLevel", "_DemoLevel", int(getEpochTimeS()));
#else
		m_pMinecraft->setScreen(new SelectWorldScreen);
#endif
	}
	else if (pButton->getId() == m_joinButton.getId())
	{
		m_pMinecraft->locateMultiplayer();
		m_pMinecraft->setScreen(new JoinGameScreen);
	}
	else if (pButton->getId() == m_buyButton.getId())
	{
#ifdef TITLE_CROP_MODE
		TitleTile::regenerate();
		return;
#endif
        
#ifdef DEMO
        m_pMinecraft->platform()->buyGame();
#else
		m_pMinecraft->quit();
#endif
	}
	else if (pButton->getId() == m_optionsButton.getId())
	{
		m_pMinecraft->getScreenChooser()->pushOptionsScreen(this);
	}
	else if (pButton->getId() == m_creditsButton.getId())
	{
		m_pMinecraft->getScreenChooser()->pushCreditsScreen(this);
	}
}

void StartMenuScreen::init()
{
	int yPos = m_height / 2;

	m_startButton.m_width = m_joinButton.m_width = 160;
	m_optionsButton.m_width = m_buyButton.m_width = 78;
	m_startButton.m_height = m_joinButton.m_height = m_optionsButton.m_height = m_buyButton.m_height = 25;

	m_joinButton.m_yPos = yPos + 25;
	m_startButton.m_yPos = yPos - 3;

	yPos += 55;

	m_optionsButton.m_yPos = yPos;
	m_buyButton.m_yPos = yPos;

	m_startButton.m_xPos = (m_width - m_startButton.m_width) / 2;

	int x1 = m_width - m_joinButton.m_width;

	m_joinButton.m_xPos = x1 / 2;
	m_optionsButton.m_xPos = x1 / 2;

	m_buyButton.m_xPos = x1 / 2 + m_optionsButton.m_width + 4;

	m_creditsButton.m_xPos = 0;
	m_creditsButton.m_yPos = 0;
	m_creditsButton.m_width = m_width;
	m_creditsButton.m_height = 75;
	m_creditsButton.m_color.a = 0.0f;

	// fill in empty space where quit/buy button would be
	if (m_pMinecraft->isTouchscreen())
	{
		m_optionsButton.m_xPos = m_startButton.m_xPos;
		m_optionsButton.m_width = m_startButton.m_width;
	}

	// add the buttons to the screen:
	_addElement(m_startButton);
	_addElement(m_joinButton);
	_addElement(m_optionsButton);

    bool canQuit = false;

#if defined(DEMO) || !MC_PLATFORM_MOBILE
	canQuit = true;
#endif

	if (canQuit)
    {
        _addElement(m_buyButton);
    }

	_addElement(m_creditsButton, false);

	m_watermarkText = "\xFFMojang AB";
	m_watermarkX = m_width - 1 - m_pFont->width(m_watermarkText);

	m_versionText = m_pMinecraft->getVersionString();
#ifdef DEMO
		" (Demo)"
#endif
		;

	m_versionTextX = (m_width - m_pFont->width(m_versionText)) / 2;

#ifndef DEMO
	m_buyButton.setMessage("Quit");
#endif

	_updateLicense();
}

bool StartMenuScreen::isInGameScreen()
{
	return false;
}

void StartMenuScreen::render(float f)
{
#ifdef TITLE_CROP_MODE
	fill(0, 0, width, height, 0xFF00FF00);
#else
	//renderBackground();
	renderMenuBackground(f);
#endif

	//int titleYPos = 4;
	//int titleYPos = 30; // -- MC Java position.
	int titleYPos = 15;
	bool crampedMode = false;

	if (m_width * 3 / 4 < 256)
	{
		crampedMode = true;
		titleYPos = 4;
	}

	LogoRenderer::singleton().render(f);

	if (m_uiTheme != UI_CONSOLE)
	{
		drawString(*m_pFont, m_versionText, m_versionTextX, 58 + titleYPos, Color(204, 204, 204));
		drawString(*m_pFont, m_watermarkText, m_watermarkX, m_height - 10, Color::WHITE);
	}

	// Draw the splash text, if we have enough room.
#ifndef TITLE_CROP_MODE
	if (!crampedMode)
		drawSplash();
#endif

	Screen::render(f);
}

void StartMenuScreen::tick()
{
	Screen::tick();
	_updateLicense();
}

void StartMenuScreen::drawSplash()
{
	MatrixStack::Ref mtx = MatrixStack::World.push();

	int textWidth = m_pFont->width(m_chosenSplash);
	mtx->translate(Vec3(float(m_width) / 2.0f + 90.0f, 70.0f, 0.0f));
	mtx->rotate(-20.0f, Vec3::UNIT_Z);

	float timeMS = float(getTimeMs() % 1000) / 1000.0f;
	float scale = 1.8f - Mth::abs(0.1f * Mth::sin(2.0f * float(M_PI) * timeMS));
	scale = (scale * 100.0f) / (32.0f + textWidth);
	mtx->scale(scale);

	drawCenteredString(*m_pFont, m_chosenSplash, 0, -8, Color::YELLOW);
}

bool StartMenuScreen::handleBackEvent(bool b)
{
	if (!b)
	{
		m_pMinecraft->quit();
	}
	return true;
}