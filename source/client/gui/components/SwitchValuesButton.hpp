#pragma once

#include "Button.hpp"

class SwitchValuesButton : public Button
{
public:
	SwitchValuesButton(int id, int x, int y, Options::ValuesOption*, const std::string&);

	void pressed(Minecraft*, int xPos, int yPos) override;
	void render(Minecraft*, int, int) override;

	Options::ValuesOption& getOption() { return *m_pOption; };

private:
	Options::ValuesOption* m_pOption;
};