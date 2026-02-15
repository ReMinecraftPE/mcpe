#pragma once

#include "Button.hpp"

class SwitchValuesButton : public Button
{
public:
	SwitchValuesButton(int x, int y, OptionEntry*, const std::string&);

	void pressed(Minecraft*) override;
	void pressed(Minecraft*, const MenuPointer& pointer) override;
	void render(Minecraft*, const MenuPointer& pointer) override;

	OptionEntry& getOption() { return *m_pOption; };

private:
	OptionEntry* m_pOption;
};