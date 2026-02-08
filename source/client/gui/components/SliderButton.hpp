#pragma once

#include "Button.hpp"

class SliderButton : public Button
{
public:
	SliderButton(int id, int x, int y, FloatOption*, const std::string&, float initialValue);

	void pressed(Minecraft*, int xPos, int yPos) override;
	int getYImage(bool bHovered) override;
	void released(int xPos, int yPos) override;
	void renderBg(Minecraft*, int, int) override;

	FloatOption& getOption() { return *m_pOption; };

private:
	FloatOption* m_pOption;

public:
	float m_value;
	bool m_bDragging;
};