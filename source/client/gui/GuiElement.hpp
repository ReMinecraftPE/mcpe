#pragma once

#include "common/math/Color.hpp"
#include "GuiComponent.hpp"

#define C_SOUND_UI_BACK     "ui.back"
#define C_SOUND_UI_FOCUS    "ui.focus"
#define C_SOUND_UI_PRESS    "ui.press"
#define C_SOUND_UI_SCROLL   "ui.scroll"

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
	GuiElement(int id);

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
	virtual void setMessage(const std::string& message);
	virtual Type getType() const { return TYPE_UNKNOWN; }
	virtual const std::string& getMessage() { return m_message; }
	virtual int getId() const { return m_ID; }
	
public:
	bool isEnabled() const { return m_bEnabled; }
	bool isVisible() const { return m_bVisible; }
	bool isSelected() const { return m_bSelected; }
	bool hasFocus() const { return m_bHasFocus; }

private:
	std::string m_message;
	int m_ID;
	bool m_bEnabled;
	bool m_bVisible;
	Color m_backgroundColor;
	bool m_bSelected;
	bool m_bHasFocus;

public:
	int m_width;
	int m_height;
	int m_xPos;
	int m_yPos;
};

