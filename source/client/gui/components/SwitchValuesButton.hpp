#pragma once

#include "Button.hpp"

class SwitchValuesButton : public Button
{
public:
	SwitchValuesButton(int id, int x, int y, ValuesOption*, const std::string&);

	void pressed(Minecraft*, const MenuPointer& pointer) override;
	void render(Minecraft*, const MenuPointer& pointer) override;

	ValuesOption& getOption() { return *m_pOption; };

private:
	ValuesOption* m_pOption;
};