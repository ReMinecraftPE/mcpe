#pragma once

#include "common/math/Color.hpp"
#include "GuiComponent.hpp"

class GuiElement : public GuiComponent
{
public:
	enum Type
	{
		TYPE_UNKNOWN,
		TYPE_BUTTON,
		TYPE_TEXTINPUT
	};

public:
	GuiElement();

public:
	void setBackground(const Color& color);

protected:
	virtual void _onSelectedChanged();
	virtual void _onFocusChanged();

public:
	virtual void setupPositions();
	virtual void setEnabled(bool isEnabled);
	virtual void setVisible(bool isVisible);
	virtual void setSelected(bool isSelected);
	virtual void setFocused(bool hasFocus);
	virtual Type getType() const { return TYPE_UNKNOWN; }
	
public:
	bool isEnabled() const { return m_bEnabled; }
	bool isVisible() const { return m_bVisible; }
	bool isSelected() const { return m_bSelected; }
	bool hasFocus() const { return m_bHasFocus; }

private:
	bool m_bEnabled;
	bool m_bVisible;
	Color m_backgroundColor;
	bool m_bSelected;
	bool m_bHasFocus;
};

