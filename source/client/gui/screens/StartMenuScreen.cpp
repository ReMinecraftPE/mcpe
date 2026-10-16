/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "StartMenuScreen.hpp"

#include "client/gui/components/LargeImageButton.hpp"
#include "client/renderer/ScreenRenderer.hpp"
#include "client/renderer/LogoRenderer.hpp"
#include "client/resources/SplashManager.hpp"

#include "InvalidLicenseScreen.hpp"
#include "SelectWorldScreen.hpp"
#include "JoinGameScreen.hpp"

#if defined(DEMO) || !MC_PLATFORM_MOBILE
#define CAN_QUIT 1
#else
#define CAN_QUIT 0
#endif

StartMenuScreen::StartMenuScreen()
	: m_buyButton(0, 0, 78, 22, "Buy")
	, m_creditsButton(0, 0, 0, 0, "")
{
	m_chosenSplash = SplashManager::singleton().getSplash();
}

StartMenuScreen::~StartMenuScreen()
{
	delete m_startButton;
	delete m_joinButton;
	delete m_optionsButton;
}

void StartMenuScreen::_updateLicense()
{
	int licenseID = m_pMinecraft->getLicenseId();
	if (licenseID < 0)
	{
		m_optionsButton->setEnabled(false);
		m_startButton->setEnabled(false);
		m_joinButton->setEnabled(false);
	}
	else if (licenseID <= 1)
	{
		m_optionsButton->setEnabled(true);
		m_startButton->setEnabled(true);
#ifdef FEATURE_NETWORKING
		m_joinButton->setEnabled(true);
#else
		m_joinButton->setEnabled(false);
#endif
	}
	else
	{
		m_pMinecraft->setScreen(new InvalidLicenseScreen(licenseID, AppPlatform::singleton()->hasBuyButtonWhenInvalidLicense()));
	}
}

void StartMenuScreen::_buttonClicked(Button* pButton)
{
	if (pButton->getId() == m_startButton->getId())
	{
#if defined(DEMO)
		m_pMinecraft->selectLevel("_DemoLevel", "_DemoLevel", int(getEpochTimeS()));
#else
		m_pMinecraft->setScreen(new SelectWorldScreen);
#endif
	}
	else if (pButton->getId() == m_joinButton->getId())
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
		AppPlatform::singleton()->buyGame();
#else
		m_pMinecraft->quit();
#endif
	}
	else if (pButton->getId() == m_optionsButton->getId())
	{
		m_pMinecraft->getScreenChooser()->pushOptionsScreen(this);
	}
	else if (pButton->getId() == m_creditsButton.getId())
	{
		m_pMinecraft->getScreenChooser()->pushCreditsScreen(this);
	}
}

void StartMenuScreen::_setup()
{
	m_startButton   = new Button(0, 0, 160, 24, "Start Game");
	m_joinButton    = new Button(0, 0, 160, 0,  "Join Game");
	m_optionsButton = new Button(0, 0,  78, 0,  "Options");

	_setupPositions();
}

void StartMenuScreen::_setupGenericPositions()
{
    // Do the exact logo bounds
    const IntRectangle& bounds = LogoRenderer::singleton().getBounds();
    m_creditsButton.m_xPos   = bounds.x;
    m_creditsButton.m_yPos   = bounds.y;
    m_creditsButton.m_width  = bounds.w;
    m_creditsButton.m_height = bounds.h;
}

void StartMenuScreen::_setupPositions()
{
	int yPos = m_height / 2;

	m_startButton->m_width = m_joinButton->m_width = 160;
	m_optionsButton->m_width = m_buyButton.m_width = 78;
    
    // Sync button heights
	m_joinButton->m_height = m_optionsButton->m_height = m_buyButton.m_height = m_startButton->m_height;

	m_joinButton->m_yPos = yPos + 25;
	m_startButton->m_yPos = yPos - 3;

	yPos += 53;

	m_optionsButton->m_yPos = yPos;
	m_buyButton.m_yPos = yPos;

	m_startButton->m_xPos = (m_width - m_startButton->m_width) / 2;

	int x1 = m_width - m_joinButton->m_width;

	m_joinButton->m_xPos = x1 / 2;
	m_optionsButton->m_xPos = x1 / 2;

	m_buyButton.m_xPos = x1 / 2 + m_optionsButton->m_width + 4;

#if !CAN_QUIT
	// fill in empty space where quit/buy button would be
	if (m_pMinecraft->useTouchscreen())
	{
		m_optionsButton->m_xPos = m_startButton->m_xPos;
		m_optionsButton->m_width = m_startButton->m_width;
	}
#endif
    
    _setupGenericPositions();
}

void StartMenuScreen::init()
{
	_setup();

	// same no matter the theme
	m_creditsButton.m_color.a = 0.0f;

	// add the buttons to the screen:
	_addElement(*m_startButton);
	_addElement(*m_joinButton);
	_addElement(*m_optionsButton);

#if CAN_QUIT
    _addElement(m_buyButton);
#endif

	_addElement(m_creditsButton, false);

	// TODO: change copyright symbol back to \xFF if utf8 is disabled
	m_watermarkText = "\xC3\xBFMojang AB";
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

	int titleYPos = 15;
	bool crampedMode = false;

	if (m_uiTheme == UI_POCKET || m_width * 3 / 4 < 256)
	{
		crampedMode = true;
		titleYPos = 4;
	}

	LogoRenderer::singleton().render(f);

	if (m_uiTheme != UI_CONSOLE)
	{
		int height = m_uiTheme == UI_POCKET ? 32 : 56;
		drawString(*m_pFont, m_versionText, m_versionTextX, (height + titleYPos) + 2, Color(204, 204, 204));
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

	float yPos = 70.0f;

	if (m_uiTheme == UI_POCKET)
		yPos -= 18.0f;

	int textWidth = m_pFont->width(m_chosenSplash);
	mtx->translate(Vec3(float(m_width) / 2.0f + 90.0f, yPos, 0.0f));
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

bool StartMenuScreen::validate(Minecraft* mc)
{
	if (mc->getOptions()->getUiTheme() == UI_CONSOLE)
	{
		mc->getScreenChooser()->pushStartScreen();
		return false;
	}
	return true;
}
