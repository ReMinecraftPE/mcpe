/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "OptionsScreen.hpp"
#include "StartMenuScreen.hpp"
#include "PauseScreen.hpp"

#ifndef OLD_OPTIONS_SCREEN

#define MIN_CATEGORY_BUTTON_ID 1
#define MAX_CATEGORY_BUTTON_ID 4
#define BACK_BUTTON_ID 100

OptionsScreen::OptionsScreen() :
	m_pList(nullptr),
	m_currentCategory(OC_MIN),
	m_videoButton(MIN_CATEGORY_BUTTON_ID, "Video"),
	m_controlsButton(MIN_CATEGORY_BUTTON_ID + 1, "Controls"),
	m_multiplayerButton(MIN_CATEGORY_BUTTON_ID + 2, "Multiplayer"),
	m_miscButton(MAX_CATEGORY_BUTTON_ID, "Misc"),
	m_backButton(BACK_BUTTON_ID, "Done")
{
	m_bRenderPointer = true;
}

OptionsScreen::~OptionsScreen()
{
	SAFE_DELETE(m_pList);
}

bool OptionsScreen::_nextTab()
{
	OptionsCategory nextCat;
	if (m_currentCategory == OC_MAX)
		nextCat = OC_MIN;
	else
		nextCat = (OptionsCategory)(m_currentCategory + 1);

	setCategory(nextCat);

	return true;
}

bool OptionsScreen::_prevTab()
{
	OptionsCategory nextCat;
	if (m_currentCategory == OC_MIN)
		nextCat = OC_MAX;
	else
		nextCat = (OptionsCategory)(m_currentCategory - 1);

	setCategory(nextCat);

	return true;
}

void OptionsScreen::init()
{
	if (m_pList)
		SAFE_DELETE(m_pList);

	m_pList = new OptionList(m_pMinecraft, m_width, m_height, 28, m_height - 28);
	
	Button* tabButtons[] = { &m_videoButton, &m_controlsButton, &m_multiplayerButton, &m_miscButton };
	constexpr int NUM_CATEGORY_BUTTONS = sizeof(tabButtons) / sizeof(tabButtons[0]);
	int buttonWidth = 64;
	int buttonHeight = 20;
	int buttonSpacing = 5;
	int totalWidth = (buttonWidth * NUM_CATEGORY_BUTTONS) + (buttonSpacing * (NUM_CATEGORY_BUTTONS - 1));
	int startX = (m_width - totalWidth) / 2;
	
	m_backButton.m_width = 100;
	m_backButton.m_height = 20;

	m_backButton.m_xPos = (m_width - m_backButton.m_width) / 2;
	m_backButton.m_yPos = m_height - m_backButton.m_height - (28 - m_backButton.m_height) / 2;
	
	for (int i = 0; i < NUM_CATEGORY_BUTTONS; ++i)
	{
		tabButtons[i]->m_width = buttonWidth;
		tabButtons[i]->m_height = buttonHeight;
		tabButtons[i]->m_xPos = startX + (buttonWidth + buttonSpacing) * i;
		tabButtons[i]->m_yPos = 4;
	}

	for (int i = 0; i < NUM_CATEGORY_BUTTONS; ++i)
	{
		_addElement(*tabButtons[i], false);
	}

	if (!_useController())
		_addElement(m_backButton);

	setCategory(m_currentCategory);
}

void OptionsScreen::render(float f)
{
	if (!m_pList)
		return;

	m_pList->render(m_menuPointer, f);

	Screen::render(f);

	//drawCenteredString(*m_pFont, "Options", m_width / 2, 10, 0xFFFFFF);
}

void OptionsScreen::removed()
{
#ifndef ORIGINAL_CODE
	m_pMinecraft->saveOptions();
#endif
}
void OptionsScreen::setCategory(OptionsCategory category)
{
	_getInternalElement(m_currentCategory)->setEnabled(true);
	m_currentCategory = category;
	_getInternalElement(m_currentCategory)->setEnabled(false);
	m_pList->clear();

	switch (category)
	{
	case OC_VIDEO:
		m_pList->initVideoMenu();
		break;
	case OC_CONTROLS:
		m_pList->initControlsMenu();
		break;
	case OC_MULTIPLAYER:
		m_pList->initMultiplayerMenu();
		break;
	case OC_MISCELLANEOUS:
		m_pList->initMiscMenu();
		break;
	}
}

void OptionsScreen::_buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId >= MIN_CATEGORY_BUTTON_ID && pButton->m_buttonId <= MAX_CATEGORY_BUTTON_ID)
	{
		setCategory((OptionsCategory)(pButton->m_buttonId - MIN_CATEGORY_BUTTON_ID));
	}
	else if (pButton->m_buttonId == BACK_BUTTON_ID)
	{
		handleBackEvent(false);
	}
}

bool OptionsScreen::handleBackEvent(bool b)
{
	if (!b)
	{
		if (m_pMinecraft->isLevelGenerated())
			m_pMinecraft->setScreen(new PauseScreen);
		else
			m_pMinecraft->setScreen(new StartMenuScreen);
	}

	return true;
}

void OptionsScreen::handleScrollWheel(float force)
{
	m_pList->handleScrollWheel(force);
}

#else

#include "client/renderer/PatchManager.hpp"
#include "client/renderer/GrassColor.hpp"
#include "client/renderer/FoliageColor.hpp"

// Note! This enum matches the order in the options screen constructor
enum eOptionsButton
{
	OB_BACK = 1,
	OB_AO,
	OB_SRV_VIS,
	OB_FANCY_GFX,
	OB_INVERT_Y,
	OB_ANAGLYPHS,
	OB_VIEW_BOB,
	OB_VIEW_DIST,
	OB_FLY_HAX,
	OB_AUTO_JUMP,
	OB_BLOCK_LINES,
	OB_FANCY_GRASS,
	OB_BIOME_COLORS
};

OptionsScreen::OptionsScreen()
#ifndef ORIGINAL_CODE
	:m_BackButton     (1, 0, 0, 200, 20, "Done"),
	m_AOButton        (2, 0, 0, 150, 20, ""),
	m_srvVisButton    (3, 0, 0, 150, 20, ""),
	m_fancyGfxButton  (4, 0, 0, 150, 20, ""),
	m_invertYButton   (5, 0, 0, 150, 20, ""),
	m_anaglyphsButton (6, 0, 0, 150, 20, ""),
	m_viewBobButton   (7, 0, 0, 150, 20, ""),
	m_viewDistButton  (8, 0, 0, 150, 20, ""),
	m_flightHaxButton (9, 0, 0, 150, 20, ""),
	m_autoJumpButton  (10, 0, 0, 150, 20, ""),
	m_blockLinesButton(11, 0, 0, 150, 20, ""),
	m_fancyGrassButton(12, 0, 0, 150, 20, ""),
	m_biomeColorsButton(13, 0, 0, 150, 20, "")
#endif
{
}

static std::string BoolOptionStr(bool b)
{
	return b ? "ON" : "OFF";
}

static std::string ViewDistanceStr(int dist)
{
	switch (dist)
	{
		case 0: return "EXTREME";
		case 1: return "FAR";
		case 2: return "NORMAL";
		case 3: return "SHORT";
		case 4: return "TINY";
		default:
		{
			std::stringstream ss;
			ss << dist;
			return ss.str();
		}
	}
}

void OptionsScreen::updateTexts()
{
	Options& o = *(m_pMinecraft->getOptions());

	m_AOButton.m_text          = "Smooth lighting: " + BoolOptionStr(o.m_bAmbientOcclusion);
	m_invertYButton.m_text     = "Invert Y-axis: "   + BoolOptionStr(o.m_bInvertMouse);
	m_viewBobButton.m_text     = "View bobbing: "    + BoolOptionStr(o.m_bViewBobbing);
	m_anaglyphsButton.m_text   = "3d Anaglyphs: "    + BoolOptionStr(o.m_bAnaglyphs);
	m_fancyGfxButton.m_text    = "Fancy graphics: "  + BoolOptionStr(o.m_bFancyGraphics);
	m_flightHaxButton.m_text   = "Flight hax: "      + BoolOptionStr(o.m_bFlyCheat);
	m_autoJumpButton.m_text    = "Auto Jump: "       + BoolOptionStr(o.m_bAutoJump);
	m_viewDistButton.m_text    = "View distance: "   + ViewDistanceStr(o.m_iViewDistance);
	m_blockLinesButton.m_text  = "Block outlines: "  + BoolOptionStr(o.m_bBlockOutlines);
	m_fancyGrassButton.m_text  = "Fancy grass: "     + BoolOptionStr(o.m_bFancyGrass);
	m_biomeColorsButton.m_text = "Biome colors: "    + BoolOptionStr(o.m_bBiomeColors);

	if (!isCramped())
		m_srvVisButton.m_text = "Server " + std::string(o.m_bServerVisibleDefault ? "visible" : "invisible") + " by default";
	else
		m_srvVisButton.m_text = "Server " + std::string(o.m_bServerVisibleDefault ? "visible" : "invisible");

	if (!(GetPatchManager()->IsGrassSidesTinted()))
	{
		m_fancyGrassButton.m_bEnabled = false;
	}
	if (!GrassColor::isAvailable() || !FoliageColor::isAvailable())
	{
		m_biomeColorsButton.m_bEnabled = false;
	}
}
bool OptionsScreen::isCramped()
{
	return m_width < 150 * 2 + 20 || m_height < 200;
}

void OptionsScreen::setWidthAllButtons(int width)
{
	m_AOButton.m_width =
	m_srvVisButton.m_width =
	m_fancyGfxButton.m_width =
	m_viewDistButton.m_width =
	m_blockLinesButton.m_width =
	m_invertYButton.m_width =
	m_anaglyphsButton.m_width =
	m_viewBobButton.m_width =
	m_flightHaxButton.m_width =
	m_autoJumpButton.m_width =
	m_fancyGrassButton.m_width = 
	m_biomeColorsButton.m_width = width;
}

void OptionsScreen::init()
{
	m_pMinecraft->platform()->showDialog(AppPlatform::DLG_OPTIONS);
	m_pMinecraft->platform()->createUserInput();

	bool crampedMode = isCramped();

	int incrementY = 25;
	int yPos = 40;
	int backGap = 12;

	// If the screen's width can't fit two buttons and a small amount of padding,
	// consider ourselves cramped.
	if (crampedMode)
	{
		crampedMode = true;
		incrementY = 22;
		yPos = 20;
		backGap = 5;
		setWidthAllButtons(125);
	}
	else
	{
		// Initialize the default buttons' widths.
		setWidthAllButtons(150);
	}

	m_BackButton.m_xPos = m_width / 2 - m_BackButton.m_width / 2;
	m_BackButton.m_height = 20;
	m_BackButton.m_yPos = m_height - m_BackButton.m_height - backGap;

	m_AOButton.m_xPos         =
	m_srvVisButton.m_xPos     = 
	m_fancyGfxButton.m_xPos   =
	m_viewDistButton.m_xPos   = 
	m_blockLinesButton.m_xPos =
	m_fancyGrassButton.m_xPos = m_width / 2 - m_AOButton.m_width - 5;

	m_invertYButton.m_xPos     =
	m_anaglyphsButton.m_xPos   =
	m_viewBobButton.m_xPos     =
	m_flightHaxButton.m_xPos   =
	m_autoJumpButton.m_xPos    = 
	m_biomeColorsButton.m_xPos = m_width / 2 + 5;

	m_AOButton.m_yPos       = m_invertYButton.m_yPos       = yPos; yPos += incrementY;
	m_srvVisButton.m_yPos   = m_anaglyphsButton.m_yPos     = yPos; yPos += incrementY;
	m_fancyGfxButton.m_yPos = m_viewBobButton.m_yPos       = yPos; yPos += incrementY;
	m_viewDistButton.m_yPos = m_flightHaxButton.m_yPos     = yPos; yPos += incrementY;
	m_autoJumpButton.m_yPos = m_blockLinesButton.m_yPos    = yPos; yPos += incrementY;
	m_fancyGrassButton.m_yPos = m_biomeColorsButton.m_yPos = yPos; yPos += incrementY;

	_addElement(m_AOButton);
	_addElement(m_srvVisButton);
	_addElement(m_fancyGfxButton);
	_addElement(m_viewDistButton);
	_addElement(m_invertYButton);
	_addElement(m_anaglyphsButton);
	_addElement(m_viewBobButton);
	_addElement(m_flightHaxButton);
	_addElement(m_autoJumpButton);
	_addElement(m_blockLinesButton);
	_addElement(m_fancyGrassButton);
	_addElement(m_biomeColorsButton);

	_addElement(m_BackButton);

	updateTexts();

#ifndef FEATURE_NETWORKING
	m_srvVisButton.m_bEnabled = false;
#endif
}

void OptionsScreen::render(int a, int b, float c)
{
	if (!m_pMinecraft->isLevelGenerated())
		renderMenuBackground(c);

	fillGradient(0, 0, m_width, m_height, 0xC0101010, 0xD0101010);

	if (m_pMinecraft->m_pPlatform->getUserInputStatus() >= 0)
	{
		m_pMinecraft->setScreen(new StartMenuScreen);
	}

#ifndef ORIGINAL_CODE
	drawCenteredString(*m_pFont, "Options", m_width / 2, isCramped() ? 5 : 20, 0xFFFFFF);

	Screen::render(a, b, c);
#endif
}

void OptionsScreen::removed()
{
#ifndef ORIGINAL_CODE
	m_pMinecraft->saveOptions();
#endif
}

#ifndef ORIGINAL_CODE

void OptionsScreen::_buttonClicked(Button* pButton)
{
	Options& o = *(m_pMinecraft->getOptions());

	bool* pOption = nullptr;
	switch (pButton->m_buttonId)
	{
		case OB_BACK:
			if (m_pMinecraft->isLevelGenerated())
				m_pMinecraft->setScreen(new PauseScreen);
			else
				m_pMinecraft->setScreen(new StartMenuScreen);
			return;

		case OB_AO:
			o.m_bAmbientOcclusion = !o.m_bAmbientOcclusion;
			Minecraft::useAmbientOcclusion = o.m_bAmbientOcclusion;
			m_pMinecraft->m_pLevelRenderer->allChanged();
			updateTexts();
			return;
		case OB_FANCY_GFX:
			o.m_bFancyGraphics ^= 1;
			m_pMinecraft->m_pLevelRenderer->allChanged();
			updateTexts();
			return;
		case OB_VIEW_DIST:
			// @TODO: fix the 'extreme'  render distance
			o.m_iViewDistance = (o.m_iViewDistance + 1) % 4;
			updateTexts();
			return;

		case OB_ANAGLYPHS:
			pOption = &o.m_bAnaglyphs;
			break;
		case OB_INVERT_Y:
			pOption = &o.m_bInvertMouse;
			break;
		case OB_SRV_VIS:
			pOption = &o.m_bServerVisibleDefault;
			break;
		case OB_VIEW_BOB:
			pOption = &o.m_bViewBobbing;
			break;
		case OB_FLY_HAX:
			pOption = &o.m_bFlyCheat;
			break;
		case OB_AUTO_JUMP:
			pOption = &o.m_bAutoJump;
			break;
		case OB_BLOCK_LINES:
			pOption = &o.m_bBlockOutlines;
			break;
		case OB_FANCY_GRASS:
			o.m_bFancyGrass ^= 1;
			m_pMinecraft->m_pLevelRenderer->allChanged();
			updateTexts();
			return;
		case OB_BIOME_COLORS:
			o.m_bBiomeColors ^= 1;
			m_pMinecraft->m_pLevelRenderer->allChanged();
			updateTexts();
			return;
	}

	if (!pOption)
		return;

	*pOption = !(*pOption);
	updateTexts();
}

#endif

#endif
