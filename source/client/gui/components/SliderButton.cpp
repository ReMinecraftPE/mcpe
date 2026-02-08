#include "SliderButton.hpp"
#include "renderer/ShaderConstants.hpp"

SliderButton::SliderButton(int id, int x, int y, FloatOption* option, const std::string& message, float initialValue) : Button(id, x, y, 150, 20, message),
	m_pOption(option),
	m_value(initialValue),
	m_bDragging(false)
{
}

void SliderButton::pressed(Minecraft* mc, int xPos, int yPos)
{
	m_value = (xPos - (m_xPos + 4)) / float(m_width - 8);

	getOption().set(m_value = Mth::clamp(m_value, 0.0f, 1.0f));
	setMessage(getOption().getMessage());
	m_bDragging = true;
}

int SliderButton::getYImage(bool bHovered)
{
	return 0;
}

void SliderButton::released(int xPos, int yPos)
{
	m_bDragging = false;
}

void SliderButton::renderBg(Minecraft* mc, int xPos, int yPos)
{
	if (!isVisible()) return;
	
	if (m_bDragging)
	{
		m_value = (xPos - (m_xPos + 4)) / float(m_width - 8);

		getOption().set(m_value = Mth::clamp(m_value, 0.0f, 1.0f));
		setMessage(getOption().getMessage());
	}

	currentShaderColor = m_color;
	blit(m_xPos + int(m_value * float(m_width - 8)), m_yPos, 0, 66, 4, m_height, 0, 0);
	blit(m_xPos + int(m_value * float(m_width - 8)) + 4, m_yPos, 196, 66, 4, m_height, 0, 0);
}
