#include "SwitchButton.hpp"
#include "client/locale/Language.hpp"
#include "renderer/ShaderConstants.hpp"

#define C_ON_OFF_SWITCH_WIDTH (38)
#define C_ON_OFF_SWITCH_HEIGHT (19)

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
        m_yPos + (m_height - 6) / 2,
        isEnabled() ? 0xCCCCCC : 0x777777);

	currentShaderColor = m_color;

	mc->m_pTextures->loadAndBindTexture("gui/touchgui.png");

	int uOffset = getOption().get() ? 160 + C_ON_OFF_SWITCH_WIDTH : 160;
	int vOffset = 206;

	blit(
		m_xPos + m_width - C_ON_OFF_SWITCH_WIDTH - 6, 
		m_yPos + (m_height - C_ON_OFF_SWITCH_HEIGHT) / 2, 
		uOffset, 
		vOffset, 
		C_ON_OFF_SWITCH_WIDTH, 
		C_ON_OFF_SWITCH_HEIGHT, 
		C_ON_OFF_SWITCH_WIDTH, 
		C_ON_OFF_SWITCH_HEIGHT
	);
}