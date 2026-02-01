/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"
#include "../IntRectangle.hpp"
#include "client/renderer/TileRenderer.hpp"

class StartMenuScreen : public Screen
{
protected:
public:
	StartMenuScreen();
	~StartMenuScreen();

protected:
	void _updateLicense();

public:
	void init() override;
	void _buttonClicked(Button*) override;
	bool isInGameScreen() override;
	void render(float f) override;
	void tick() override;

	void drawSplash();

	std::string getSplashString();

	bool handleBackEvent(bool b) override;

protected:
	Button m_startButton;
	Button m_joinButton;
	Button m_optionsButton;
	Button m_buyButton;
	Button m_creditsButton;
	std::string m_watermarkText;
	int m_watermarkX;
	std::string m_versionText;
	int m_versionTextX;

	int m_chosenSplash;
};

