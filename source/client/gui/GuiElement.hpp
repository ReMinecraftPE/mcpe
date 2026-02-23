#pragma once

#include "common/math/Color.hpp"
#include "client/gui/MenuPointer.hpp"
#include "GuiComponent.hpp"
#include "AreaNavigation.hpp"

#define C_SOUND_UI_BACK     "ui.back"
#define C_SOUND_UI_FOCUS    "ui.focus"
#define C_SOUND_UI_PRESS    "ui.press"
#define C_SOUND_UI_SCROLL   "ui.scroll"

class Minecraft;

class GuiElement : public GuiComponent
{
public:
	typedef int ID;

public:
	enum Type
	{
		TYPE_UNKNOWN,
		TYPE_BUTTON,
		TYPE_TEXTBOX
	};

public:
	GuiElement();

public:
	void setBackground(const Color& color);

protected:
	virtual void _onSelectedChanged();
	virtual void _onFocusChanged();
	virtual bool _isHovered(const MenuPointer& pointer);

public:
	virtual void setupPositions();
	virtual void tick(Minecraft* pMinecraft);
	virtual bool pointerPressed(Minecraft* pMinecraft, const MenuPointer& pointer);
	virtual bool pointerReleased(Minecraft* pMinecraft, const MenuPointer& pointer);
	virtual bool areaNavigation(Minecraft* pMinecraft, AreaNavigation::Direction);
	virtual void handleButtonPress(Minecraft* pMinecraft, int key);
	virtual void handleTextChar(Minecraft* pMinecraft, int chr);
	virtual void handleClipboardPaste(const std::string& content);
	virtual void handleScroll(float force);
	virtual bool isHovered(Minecraft*, const MenuPointer& pointer);
	virtual void pressed(Minecraft* pMinecraft);
	virtual void pressed(Minecraft*, const MenuPointer& pointer);
	virtual void released(const MenuPointer& pointer);
	virtual void render(Minecraft* pMinecraft, const MenuPointer& pointer);
	virtual void setEnabled(bool isEnabled);
	virtual void setVisible(bool isVisible);
	virtual void setSelected(bool isSelected);
	virtual void setFocused(bool hasFocus);
	virtual void setId(ID);
	virtual void setNavigable(bool isNavigable);
	virtual void setMessage(const std::string& message);
	virtual void setTextboxText(const std::string& text);
	virtual Type getType() const { return TYPE_UNKNOWN; }
	virtual const std::string& getMessage() { return m_message; }
	virtual ID getId() const { return m_ID; }
	
public:
	bool isEnabled() const { return m_bEnabled; }
	bool isVisible() const { return m_bVisible; }
	bool isSelected() const { return m_bSelected; }
	bool hasFocus() const { return m_bHasFocus; }
	bool isNavigable() const { return m_bNavigable; }

private:
	std::string m_message;
	ID m_ID;
	bool m_bEnabled;
	bool m_bVisible;
	Color m_backgroundColor;
	bool m_bSelected;
	bool m_bHasFocus;
	bool m_bNavigable;

public:
	int m_width;
	int m_height;
	int m_xPos;
	int m_yPos;
	UITheme m_uiTheme;
};

