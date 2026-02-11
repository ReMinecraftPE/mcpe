#include "GuiElement.hpp"

GuiElement::GuiElement()
{
	m_ID = -1;
	m_uiTheme = UI_POCKET;
	m_width = 0;
	m_height = 0;
	m_xPos = 0;
	m_yPos = 0;
	m_bEnabled = true;
	m_bVisible = true;
	m_bSelected = false;
	m_bHasFocus = false;
	m_bNavigable = true;
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

bool GuiElement::_isHovered(const MenuPointer& pointer)
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

bool GuiElement::areaNavigation(Minecraft* pMinecraft, AreaNavigation::Direction)
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

void GuiElement::handleScroll(float force)
{
}

bool GuiElement::isHovered(Minecraft* pMinecraft, const MenuPointer& pointer)
{
	return _isHovered(pointer);
}

void GuiElement::pressed(Minecraft* pMinecraft)
{
}

void GuiElement::pressed(Minecraft* pMinecraft, const MenuPointer& pointer)
{
	pressed(pMinecraft);
}

void GuiElement::released(const MenuPointer& pointer)
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

void GuiElement::setId(ID id)
{
	m_ID = id;
}

void GuiElement::setNavigable(bool isNavigable)
{
	m_bNavigable = isNavigable;
}

void GuiElement::setMessage(const std::string& message)
{
	m_message = message;
}

void GuiElement::setTextboxText(const std::string& text)
{
}
