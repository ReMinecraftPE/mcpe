#pragma once

#include "Button.hpp"

class SwitchButton : public Button
{
public:
	SwitchButton(int id, int x, int y, BoolOption*, const std::string&);

	void pressed(Minecraft*, const MenuPointer& pointer) override;
	void render(Minecraft*, const MenuPointer& pointer) override;

	BoolOption& getOption() { return *m_pOption; };

private:
	BoolOption* m_pOption;
};