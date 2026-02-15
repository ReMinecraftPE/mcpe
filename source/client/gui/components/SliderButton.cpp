#include "SliderButton.hpp"
#include "renderer/ShaderConstants.hpp"

SliderButton::SliderButton(int x, int y, int width, int height, OptionEntry* option, const std::string& message, float initialValue) : Button(x, y, width, height, message),
	m_pOption(option),
	m_value(initialValue),
	m_bDragging(false)
{
}

bool SliderButton::areaNavigation(Minecraft* pMinecraft, AreaNavigation::Direction dir)
{
	float oldValue = m_value;
	if (dir == AreaNavigation::LEFT)
	{
		getOption().addUnit(-1);
		updateValue();
	}
	else if (dir == AreaNavigation::RIGHT)
	{
		getOption().addUnit(1);
		updateValue();
	}

	if (oldValue != m_value)
	{
		pMinecraft->m_pSoundEngine->playUI(C_SOUND_UI_SCROLL);
		return true;
	}

	return false;
}

void SliderButton::pressed(Minecraft* mc, const MenuPointer& pointer)
{
	m_value = (pointer.x - (m_xPos + 4)) / float(m_width - 8);

	getOption().fromFloat(m_value = Mth::clamp(m_value, 0.0f, 1.0f));
	m_bDragging = true;
}

int SliderButton::getYImage(bool bHovered)
{
	return 0;
}

void SliderButton::released(const MenuPointer& pointer)
{
	m_bDragging = false;
}

void SliderButton::renderBg(Minecraft* mc, const MenuPointer& pointer)
{
	if (m_uiTheme != UI_CONSOLE)
		Button::renderBg(mc, pointer);
	
	if (m_bDragging)
	{
		m_value = (pointer.x - (m_xPos + 4)) / float(m_width - 8);

		getOption().fromFloat(m_value = Mth::clamp(m_value, 0.0f, 1.0f));
		updateValue();
	}

	currentShaderColor = m_color;

	if (m_uiTheme == UI_CONSOLE)
	{
		Textures& texs = *mc->m_pTextures;
		blitSprite(texs, "gui/console/Graphics/Slider_Track.png", m_xPos, m_yPos, m_width - 2, m_height, nullptr, 0.0f, 0.0f, m_width - 2);
		blitSprite(texs, "gui/console/Graphics/Slider_Track.png", m_xPos + m_width - 2, m_yPos, 2, m_height, nullptr, 0.0f, 0.0f, 2.0f);
		if (isSelected())
			blitNineSlice(texs, "gui/slider_highlight.png", m_xPos - 3, m_yPos - 3, m_width + 6, m_height + 6, 5);
		blitSprite(texs, "gui/console/Graphics/Slider_Button.png", m_xPos + int(m_value * float(m_width - 16)), m_yPos, 16, m_height);
	}
	else
	{
		blit(m_xPos + int(m_value * float(m_width - 8)), m_yPos, 0, 66, 4, m_height, 0, 20);
		blit(m_xPos + int(m_value * float(m_width - 8)) + 4, m_yPos, 196, 66, 4, m_height, 0, 20);
	}
}

void SliderButton::updateValue()
{
	setMessage(getOption().getMessage());
	m_value = getOption().toFloat();
}
