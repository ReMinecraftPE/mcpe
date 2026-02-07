#pragma once

#include "Button.hpp"

class SwitchButton : public Button
{
public:
	SwitchButton(int id, int x, int y, Options::BoolOption*, const std::string&);

	void pressed(Minecraft*, int xPos, int yPos) override;
	void render(Minecraft*, int, int) override;

	Options::BoolOption& getOption() { return *m_pOption; };

private:
	Options::BoolOption* m_pOption;
};