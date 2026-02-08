/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"
#include "../components/Button.hpp"
#include "../components/WorldSelectionList.hpp"

class SelectWorldScreen : public Screen
{
public:
	SelectWorldScreen();
    ~SelectWorldScreen();

protected:
	void _controllerDirectionHeld(GameController::StickID stickId, GameController::StickState stickState) override;

public:
	void init() override;
	bool isInGameScreen() override;
	void keyPressed(int code) override;
	void tick() override;
	void render(float f) override;
	bool handleBackEvent(bool b) override;
	void _buttonClicked(Button* pButton) override;
	void handleScrollWheel(float force) override;

public:
	bool isIndexValid(int);
	std::string getUniqueLevelName(const std::string& in);
	void loadLevelSource();

public:
	Button m_btnDelete;
	Button m_btnCreateNew;
	Button m_btnBack;
	Button m_btnWorld;
	WorldSelectionList* m_pWorldSelectionList;
	std::vector<LevelSummary> m_levels;
	bool field_12C;
	int field_130;
};

