/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "OptionsScreen.hpp"
#include "StartMenuScreen.hpp"
#include "PauseScreen.hpp"

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
};

OptionsScreen::OptionsScreen()
#ifndef ORIGINAL_CODE
	:m_BackButton    (1, 0, 0, 200, 20, "Done"),
	m_AOButton       (2, 0, 0, 150, 20, ""),
	m_srvVisButton   (3, 0, 0, 150, 20, ""),
	m_fancyGfxButton (4, 0, 0, 150, 20, ""),
	m_invertYButton  (5, 0, 0, 150, 20, ""),
	m_anaglyphsButton(6, 0, 0, 150, 20, ""),
	m_viewBobButton  (7, 0, 0, 150, 20, ""),
	m_viewDistButton (8, 0, 0, 150, 20, ""),
	m_flightHaxButton(9, 0, 0, 150, 20, "")
#endif
{
}

#ifndef ORIGINAL_CODE
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

void OptionsScreen::UpdateTexts()
{
	Options& o = m_pMinecraft->m_options;

	m_AOButton.field_18        = "Smooth lighting: " + BoolOptionStr(o.field_18);
	m_invertYButton.field_18   = "Invert Y-axis: "   + BoolOptionStr(o.m_bInvertMouse);
	m_viewBobButton.field_18   = "View bobbing: "    + BoolOptionStr(o.field_14);
	m_anaglyphsButton.field_18 = "3d Anaglyphs: "    + BoolOptionStr(o.m_bAnaglyphs);
	m_fancyGfxButton.field_18  = "Fancy graphics: "  + BoolOptionStr(o.m_bFancyGraphics);
	m_flightHaxButton.field_18 = "Flight hax: "      + BoolOptionStr(o.m_bFlyCheat);
	m_viewDistButton.field_18  = "View distance: "   + ViewDistanceStr(o.field_10);
	m_srvVisButton.field_18    = "Server " + std::string(o.m_bServerVisibleDefault ? "visible" : "invisible") + " by default";
}
#endif

void OptionsScreen::init()
{
	m_pMinecraft->platform()->showDialog(AppPlatform::DLG_OPTIONS);
	m_pMinecraft->platform()->createUserInput();

#ifndef ORIGINAL_CODE
	m_BackButton.m_xPos = m_width / 2 - m_BackButton.m_width / 2;
	m_BackButton.m_yPos = m_height - 33;
	m_BackButton.m_height = 20;
	m_buttons.push_back(&m_BackButton);

	m_AOButton.m_xPos       =
	m_srvVisButton.m_xPos   = 
	m_fancyGfxButton.m_xPos =
	m_viewDistButton.m_xPos = m_width / 2 - m_AOButton.m_width - 5;

	m_invertYButton.m_xPos   =
	m_anaglyphsButton.m_xPos =
	m_viewBobButton.m_xPos   = 
	m_flightHaxButton.m_xPos = m_width / 2 + 5;

	int yPos = 40;
	m_AOButton.m_yPos       = m_invertYButton.m_yPos   = yPos; yPos += 25;
	m_srvVisButton.m_yPos   = m_anaglyphsButton.m_yPos = yPos; yPos += 25;
	m_fancyGfxButton.m_yPos = m_viewBobButton.m_yPos   = yPos; yPos += 25;
	m_viewDistButton.m_yPos = m_flightHaxButton.m_yPos = yPos; yPos += 25;

	m_buttons.push_back(&m_AOButton);
	m_buttons.push_back(&m_srvVisButton);
	m_buttons.push_back(&m_fancyGfxButton);
	m_buttons.push_back(&m_invertYButton);
	m_buttons.push_back(&m_anaglyphsButton);
	m_buttons.push_back(&m_viewBobButton);
	m_buttons.push_back(&m_viewDistButton);
	m_buttons.push_back(&m_flightHaxButton);

	m_buttonTabList.push_back(&m_AOButton);
	m_buttonTabList.push_back(&m_srvVisButton);
	m_buttonTabList.push_back(&m_fancyGfxButton);
	m_buttonTabList.push_back(&m_viewDistButton);
	m_buttonTabList.push_back(&m_invertYButton);
	m_buttonTabList.push_back(&m_anaglyphsButton);
	m_buttonTabList.push_back(&m_viewBobButton);
	m_buttonTabList.push_back(&m_flightHaxButton);

	m_buttonTabList.push_back(&m_BackButton);

	UpdateTexts();
#endif
}

void OptionsScreen::render(int a, int b, float c)
{
	renderBackground();

	if (m_pMinecraft->m_pPlatform->getUserInputStatus() >= 0)
	{
		m_pMinecraft->setScreen(new StartMenuScreen);
	}

#ifndef ORIGINAL_CODE
	drawCenteredString(m_pFont, "Options", m_width / 2, 20, 0xFFFFFF);

	Screen::render(a, b, c);
#endif
}

void OptionsScreen::removed()
{
#ifdef ORIGINAL_CODE // Reloading options will reload the options.txt we introduced. Don't do this
	m_pMinecraft->reloadOptions();
#endif
}

#ifndef ORIGINAL_CODE

void OptionsScreen::buttonClicked(Button* pButton)
{
	Options& o = m_pMinecraft->m_options;

	bool* pOption = nullptr;
	switch (pButton->field_30)
	{
		case OB_BACK:
			if (m_pMinecraft->isLevelGenerated())
				m_pMinecraft->setScreen(new PauseScreen);
			else
				m_pMinecraft->setScreen(new StartMenuScreen);
			return;

		case OB_AO:
			o.field_18 ^= 1;
			Minecraft::useAmbientOcclusion = o.field_18;
			m_pMinecraft->m_pLevelRenderer->allChanged();
			UpdateTexts();
			return;
		case OB_FANCY_GFX:
			o.m_bFancyGraphics ^= 1;
			m_pMinecraft->m_pLevelRenderer->allChanged();
			UpdateTexts();
			return;
		case OB_VIEW_DIST:
			// @TODO: fix the 'extreme'  render distance
			o.field_10 = (o.field_10 + 1) % 4;
			UpdateTexts();
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
			pOption = &o.field_14;
			break;
		case OB_FLY_HAX:
			pOption = &o.m_bFlyCheat;
			break;
	}

	if (!pOption)
		return;

	*pOption ^= 1;
	UpdateTexts();
}

#endif
