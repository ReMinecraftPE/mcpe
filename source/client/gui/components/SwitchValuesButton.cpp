#include "SwitchValuesButton.hpp"
#include "renderer/ShaderConstants.hpp"
#include "client/locale/Language.hpp"

#define C_SWITCH_VALUES_WIDTH (60)
#define C_SWITCH_VALUES_HEIGHT (18)

SwitchValuesButton::SwitchValuesButton(int x, int y, OptionEntry* option, const std::string& message) : Button(x, y, 150, 20, message),
	m_pOption(option)
{
}

void SwitchValuesButton::pressed(Minecraft*)
{
	if (!isEnabled()) return;
	getOption().toggle();
}

void SwitchValuesButton::pressed(Minecraft* mc, const MenuPointer& pointer)
{
	if (pointer.x >= m_xPos + m_width - C_SWITCH_VALUES_WIDTH - 6 && pointer.x < m_xPos + m_width - 6)
	{
		pressed(mc);
	}
}

void SwitchValuesButton::render(Minecraft* mc, const MenuPointer& pointer)
{
	if (!isVisible()) return;

	drawString(
		*mc->m_pFont,
		getMessage(),
		m_xPos + 6,
		m_yPos + (m_height - 8) / 2,
		Color(204, 204, 204));

	currentShaderColor = m_color;

	int x = m_xPos + m_width - C_SWITCH_VALUES_WIDTH - 6;
	int y = m_yPos + (m_height - C_SWITCH_VALUES_HEIGHT) / 2;

	fill(x + 0, y + 0, x + C_SWITCH_VALUES_WIDTH - 0, y + C_SWITCH_VALUES_HEIGHT - 0, 0xFF444444);
	fill(x + 1, y + 1, x + C_SWITCH_VALUES_WIDTH - 1, y + C_SWITCH_VALUES_HEIGHT - 1, 0xFF111111);

	drawCenteredString(
		*mc->m_pFont,
		Language::get(getOption().getDisplayValue()),
		x + C_SWITCH_VALUES_WIDTH / 2,
		y + (C_SWITCH_VALUES_HEIGHT - 8) / 2,
		0xFFFFFF
	);
}
