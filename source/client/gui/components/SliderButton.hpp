#pragma once

#include "Button.hpp"

class SliderButton : public Button
{
public:
	SliderButton(int id, int x, int y, FloatOption*, const std::string&, float initialValue);

	void pressed(Minecraft*, const MenuPointer& pointer) override;
	int getYImage(bool bHovered) override;
	void released(const MenuPointer& pointer) override;
	void renderBg(Minecraft*, const MenuPointer& pointer) override;

	FloatOption& getOption() { return *m_pOption; };

private:
	FloatOption* m_pOption;

public:
	float m_value;
	bool m_bDragging;
};