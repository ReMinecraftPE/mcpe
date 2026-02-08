/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"
#include "client/gui/components/Button.hpp"

#ifndef OLD_OPTIONS_SCREEN

#include "../components/OptionList.hpp"
enum OptionsCategory
{
	OC_VIDEO,
	OC_CONTROLS,
	OC_GAMEPLAY,
	OC_MISCELLANEOUS,

	OC_MIN = OC_VIDEO,
	OC_MAX = OC_MISCELLANEOUS,
	OC_COUNT
};

class OptionsScreen : public Screen
{
public:
	OptionsScreen(Screen*);
	~OptionsScreen();

protected:
	bool _nextTab() override;
	bool _prevTab() override;

public:
	void init() override;
	void render(float f) override;
	void removed() override;
	void _buttonClicked(Button* pButton) override;
	bool handleBackEvent(bool b) override;
	void handleScrollWheel(float force) override;

private:
	void setCategory(OptionsCategory category);

	Screen* m_pParent;
	OptionList* m_pList;
	OptionsCategory m_currentCategory;

	Button m_videoButton;
	Button m_controlsButton;
	Button m_gameplayButton;
	Button m_miscButton;

	Button m_backButton;
};

#else

class OptionsScreen : public Screen
{
public:
	OptionsScreen();
	void init() override;
	void render(float) override;
	void removed() override;

#ifndef ORIGINAL_CODE
	void _buttonClicked(Button* pButton) override;

	void setWidthAllButtons(int width);
	void updateTexts();
	bool isCramped();

private:
	Button m_BackButton;
	Button m_AOButton;
	Button m_srvVisButton;
	Button m_fancyGfxButton;
	Button m_invertYButton;
	Button m_anaglyphsButton;
	Button m_viewBobButton;
	Button m_viewDistButton;
	Button m_flightHaxButton;
	Button m_autoJumpButton;
	Button m_blockLinesButton;
	Button m_fancyGrassButton;
	Button m_biomeColorsButton;
#endif
};

#endif
