#include "GuiElement.hpp"

GuiElement::GuiElement(GuiElement::ID id)
{
	m_ID = id;
	m_uiTheme = UI_POCKET;
	m_width = 0;
	m_height = 0;
	m_xPos = 0;
	m_yPos = 0;
	m_bEnabled = true;
	m_bVisible = true;
	m_bSelected = false;
	m_bHasFocus = false;
}

void GuiElement::setBackground(const Color& color)
{
	m_backgroundColor = color;
}

void GuiElement::_onSelectedChanged()
{
}

void GuiElement::_onFocusChanged()
{
}

bool GuiElement::_clicked(const MenuPointer& pointer)
{
	if (!isEnabled()) return false;

	if (pointer.x < m_xPos) return false;
	if (pointer.y < m_yPos) return false;
	if (pointer.x >= m_xPos + m_width) return false;
	if (pointer.y >= m_yPos + m_height) return false;

	return true;
}

void GuiElement::setupPositions()
{
}

void GuiElement::tick(Minecraft* pMinecraft)
{
}

bool GuiElement::pointerPressed(Minecraft* pMinecraft, const MenuPointer& pointer)
{
	return false;
}

bool GuiElement::pointerReleased(Minecraft* pMinecraft, const MenuPointer& pointer)
{
	return false;
}

void GuiElement::handleButtonPress(Minecraft* pMinecraft, int key)
{
}

void GuiElement::handleTextChar(Minecraft* pMinecraft, int chr)
{
}

void GuiElement::handleClipboardPaste(const std::string& content)
{
}

void GuiElement::render(Minecraft* pMinecraft, const MenuPointer& pointer)
{
}

void GuiElement::setEnabled(bool value)
{
	m_bEnabled = value;
}

void GuiElement::setVisible(bool value)
{
	m_bVisible = value;
}

void GuiElement::setSelected(bool value)
{
	bool prev = isSelected();
	m_bSelected = value;
	if (prev != value)
		_onSelectedChanged();
}

void GuiElement::setFocused(bool value)
{
	bool prev = hasFocus();
	m_bHasFocus = value;
	if (prev != value)
		_onFocusChanged();
}

void GuiElement::setMessage(const std::string& message)
{
	m_message = message;
}
