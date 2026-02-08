#include "SwitchButton.hpp"
#include "client/locale/Language.hpp"
#include "renderer/ShaderConstants.hpp"

#define C_ON_OFF_SWITCH_WIDTH (32)
#define C_ON_OFF_SWITCH_HEIGHT (18)

SwitchButton::SwitchButton(int id, int x, int y, BoolOption* option, const std::string& message) : Button(id, x, y, 150, 20, message),
	m_pOption(option)
{
}

void SwitchButton::pressed(Minecraft* mc, const MenuPointer& pointer)
{
	if (pointer.x >= m_xPos + m_width - C_ON_OFF_SWITCH_WIDTH - 6 && pointer.x < m_xPos + m_width - 6)
	{
		getOption().toggle();
	}
}

void SwitchButton::render(Minecraft* mc, const MenuPointer& pointer)
{
	if (!isVisible()) return;

	drawString(
		*mc->m_pFont,
		getMessage(),
		m_xPos + 6,
		m_yPos + (m_height - 8) / 2,
		isEnabled() ? 0xCCCCCC : 0x777777);

	currentShaderColor = m_color;

	// Obs: The color setting that affected disabled items' texts will also affect this.

	mc->m_pTextures->loadAndBindTexture("gui/gui_custom.png");

	blit(m_xPos + m_width - C_ON_OFF_SWITCH_WIDTH - 6, m_yPos + (m_height - C_ON_OFF_SWITCH_HEIGHT) / 2, 0, getOption().get() ? 0 : 18, C_ON_OFF_SWITCH_WIDTH, C_ON_OFF_SWITCH_HEIGHT, C_ON_OFF_SWITCH_WIDTH, C_ON_OFF_SWITCH_HEIGHT);
}
