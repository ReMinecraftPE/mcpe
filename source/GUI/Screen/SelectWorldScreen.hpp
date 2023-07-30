#pragma once

#include "Screen.hpp"
#include "WorldSelectionList.hpp"

class SelectWorldScreen : public Screen
{
public:
	SelectWorldScreen();

	void init() override;
	bool isInGameScreen() override;
	void keyPressed(int code) override;
	void tick() override;
	void render(int mouseX, int mouseY, float f) override;
	bool handleBackEvent(bool b) override;
	void buttonClicked(Button* pButton) override;

	bool isIndexValid(int);
	std::string getUniqueLevelName(const std::string& in);
	void loadLevelSource();

public:
	Button m_btnDelete;
	Button m_btnCreateNew;
	Button m_btnBack;
	Button m_btnUnknown;
	WorldSelectionList* m_pWorldSelectionList = nullptr;
	std::vector<LevelSummary> m_levels;
	bool field_12C;
	int field_130 = 0;
};

