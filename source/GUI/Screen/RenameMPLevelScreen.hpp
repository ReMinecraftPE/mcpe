#pragma once

#include "Screen.hpp"

class RenameMPLevelScreen : public Screen
{
public:
	RenameMPLevelScreen(const std::string& levelName);
	void init() override;
	void render(int mouseX, int mouseY, float f) override;

private:
	std::string m_levelName;
};

