/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"

#ifndef OLD_OPTIONS_SCREEN

#include "../components/OptionList.hpp"

class OptionsScreen : public Screen
{
public:
	OptionsScreen();
	~OptionsScreen();
	void init() override;
	void render(int, int, float) override;
	void removed() override;
	void buttonClicked(Button* pButton) override;
	bool handleBackEvent(bool b) override;
	void handleScroll(bool down) override;

private:
	OptionList* m_pList;

	Button m_backButton;
};

#else

class OptionsScreen : public Screen
{
public:
	OptionsScreen();
	void init() override;
	void render(int, int, float) override;
	void removed() override;

#ifndef ORIGINAL_CODE
	void buttonClicked(Button* pButton) override;

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
