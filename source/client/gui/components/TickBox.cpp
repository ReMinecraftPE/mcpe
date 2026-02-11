#include "TickBox.hpp"
#include "client/app/Minecraft.hpp"
#include "renderer/ShaderConstants.hpp"

#define C_TICKBOX_SIZE (24)
#define C_DEFAULT_TICKBOX_WIDTH (200)
#define C_DEFAULT_TICKBOX_HEIGHT (27)

TickBox::TickBox(int x, int y, BoolOption* option, const std::string& message) :
	m_pOption(option)
{
	m_xPos = x;
	m_yPos = y;
	m_width = C_DEFAULT_TICKBOX_WIDTH;
	m_height = C_DEFAULT_TICKBOX_HEIGHT;
	setMessage(message);
}

void TickBox::pressed(Minecraft* mc, const MenuPointer& pointer)
{
	pressed(mc);
}

void TickBox::pressed(Minecraft* mc)
{
	if (!isEnabled()) return;
	getOption().toggle();
}

void TickBox::render(Minecraft* mc, const MenuPointer& pointer)
{
	if (!isVisible()) return;

	if (!mc->m_pScreen->doElementTabbing())
		setSelected(isHovered(mc, pointer));

	if (!isEnabled())
		currentShaderColor.a *= 0.5f;

	mc->m_pFont->drawScalable(
		getMessage(),
		m_xPos + C_TICKBOX_SIZE + 5,
		m_yPos + m_height / 2 - 8,
		Color::GREY_TEXT);

	if (isSelected())
		mc->m_pFont->drawScalable(
			getMessage(),
			m_xPos + C_TICKBOX_SIZE + 4,
			m_yPos + m_height / 2 - 9,
			Color(204, 196, 13));

	blitSprite(*mc->m_pTextures, isSelected() ? "gui/console/Graphics/Tickbox_Over.png" : "gui/console/Graphics/Tickbox_Norm.png", m_xPos, m_yPos + (m_height - C_TICKBOX_SIZE) / 2, C_TICKBOX_SIZE, C_TICKBOX_SIZE, &m_materials.ui_textured_and_glcolor);

	if (getOption().get())
		blitSprite(*mc->m_pTextures, "gui/console/Graphics/Tick.png", m_xPos, m_yPos + (m_height - C_TICKBOX_SIZE) / 2, 28, 24, &m_materials.ui_textured_and_glcolor);

	currentShaderColor = Color::WHITE;
}
