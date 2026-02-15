#pragma once

#include "../GuiElement.hpp"

class TickBox : public GuiElement
{
public:
	TickBox(int x, int y, BoolOption*, const std::string&);
	TickBox(int x, int y, bool, const std::string&);

	void pressed(Minecraft*, const MenuPointer& pointer) override;
	void pressed(Minecraft*) override;
	void render(Minecraft*, const MenuPointer& pointer) override;

	BoolOption& getOption() { return *m_pOption; };

private:
	BoolOption* m_pOption;

public:
	bool m_bOn;
};