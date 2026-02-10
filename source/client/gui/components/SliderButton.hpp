#pragma once

#include "Button.hpp"

class SliderButton : public Button
{
public:
	SliderButton(int x, int y, int width, int height, OptionEntry*, const std::string&, float initialValue);

	bool areaNavigation(Minecraft* pMinecraft, AreaNavigation::Direction) override;
	void pressed(Minecraft*, const MenuPointer& pointer) override;
	int getYImage(bool bHovered) override;
	void released(const MenuPointer& pointer) override;
	void renderBg(Minecraft*, const MenuPointer& pointer) override;

	void updateValue();

	OptionEntry& getOption() { return *m_pOption; };

private:
	OptionEntry* m_pOption;

public:
	float m_value;
	bool m_bDragging;
};