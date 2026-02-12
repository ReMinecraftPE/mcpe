#pragma once

#include "../Screen.hpp"

class ConsoleProgressScreen : public Screen
{
public:
	void render(float f) override;
	void updateEvents() override;
	bool isInGameScreen() override;
	void init() override;

private:
	std::string m_header;

	std::string m_loadingTip;
	std::vector<std::string> m_loadingTipLines;
};

