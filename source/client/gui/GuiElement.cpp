#include "GuiElement.hpp"

GuiElement::GuiElement(int id)
{
	m_ID = id;
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

void GuiElement::setupPositions()
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
