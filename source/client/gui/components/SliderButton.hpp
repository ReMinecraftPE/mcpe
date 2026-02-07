#pragma once

#include "Button.hpp"

class SliderButton : public Button
{
public:
	SliderButton(int id, int x, int y, Options::FloatOption*, const std::string&, float initialValue);

	void pressed(Minecraft*, int xPos, int yPos) override;
	int getYImage(bool bHovered) override;
	void released(int xPos, int yPos) override;
	void renderBg(Minecraft*, int, int) override;

	Options::FloatOption& getOption() { return *m_pOption; };

private:
	Options::FloatOption* m_pOption;

public:
	float m_value;
	bool m_bDragging;
};