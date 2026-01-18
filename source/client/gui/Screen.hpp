/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "renderer/MaterialPtr.hpp"
#include "client/player/input/Mouse.hpp"
#include "client/player/input/Keyboard.hpp"
#include "client/player/input/GameController.hpp"
#include "components/Button.hpp"
#include "components/TextInputBox.hpp"
#include "MenuPointer.hpp"

class Button;
class TextInputBox;

typedef std::vector<GuiElement*> GuiElementList;

class Screen : public GuiComponent
{
protected:
	class Materials
	{
	public:
		mce::MaterialPtr ui_cubemap;
		mce::MaterialPtr ui_background;

		Materials();
	};

private:
	static bool _isPanoramaAvailable;
public:
	static bool isMenuPanoramaAvailable()
	{
#ifdef ENH_MENU_BACKGROUND
		return _isPanoramaAvailable;
#else
		return false;
#endif
	}
	static void setIsMenuPanoramaAvailable(bool value) { _isPanoramaAvailable = value; }

public:
	Screen();
	virtual ~Screen();

protected:
	bool _prevElement();
	bool _nextElement();
	void _addElement(Button& element, bool isTabbable = true);
	void _addElementToList(unsigned int index, Button& element, bool isTabbable = true);
	bool _nextElementList();
	bool _prevElementList();
	void _addElementList();
	void _selectCurrentElement();
	void _deselectCurrentElement();
	void _renderPointer();
	GuiElement* _getInternalElement(unsigned int index);
	GuiElement* _getElement(unsigned int index);
	GuiElement* _getSelectedElement();
	GuiElementList& _getElementList(unsigned int index);
	GuiElementList& _getElementList();
	bool _useController() const;

public:
	void init(Minecraft*, int, int);
	void setSize(int width, int height);
	void onRender(float f);
	int getYOffset() const;
	int getCursorMoveThrottle() const { return 65; }
	bool doElementTabbing() const;

protected:
	virtual void _processControllerDirection(GameController::StickID stickId);
	virtual void _controllerDirectionChanged(GameController::StickID stickId, GameController::StickState stickState);
	virtual void _controllerDirectionHeld(GameController::StickID stickId, GameController::StickState stickState);
	virtual void _buttonClicked(Button* pButton);
	virtual void _guiElementClicked(GuiElement& element);
	virtual void _updateTabButtonSelection();
	virtual void _nextTab();
	virtual void _prevTab();

public:
	virtual void render(float a);
	virtual void init() {};
	virtual void updateEvents();
	virtual void mouseEvent();
	virtual void keyboardEvent();
	virtual void controllerEvent();
	virtual void checkForPointerEvent();
	virtual bool handleBackEvent(bool b) { return false; }
	virtual void handlePointerLocation(MenuPointer::Unit x, MenuPointer::Unit y);
	virtual void handlePointerPressed(bool isPressed);
	virtual void handlePointerAction(const MenuPointer& pointer);
	virtual void handleScrollWheel(float force);
	virtual void handleControllerStickEvent(const GameController::StickEvent& stick);
	virtual void tick();
	virtual void removed() {};
	virtual void renderBackground(int vo);
	virtual void renderBackground();
	virtual void renderDirtBackground(int vo);
	virtual bool isPauseScreen() { return true; }
	virtual bool isErrorScreen() { return false; }
	virtual bool isInGameScreen() { return true; }
	virtual void confirmResult(bool b, int i) {};
	virtual void onTextBoxUpdated(int id) {};
	virtual void pointerPressed(int x, int y, MouseButtonType btn);
	virtual void pointerReleased(int x, int y, MouseButtonType btn);
	virtual void keyPressed(int);
	virtual void keyboardNewChar(char);
	virtual void keyboardTextPaste(const std::string& text);

	// ported from 0.8
	virtual void renderMenuBackground(float f);

protected:
	Materials m_screenMaterials;
	MenuPointer m_menuPointer;

public:
	int m_width;
	int m_height;
	bool field_10;
	Minecraft* m_pMinecraft;
	GuiElementList m_elements;
	std::vector<GuiElementList> m_elementTabLists; 
	unsigned int m_elementListIndex;
	unsigned int m_elementIndex;
	bool m_bTabWrap;
	Font* m_pFont;
	Button* m_pClickedButton;

#ifndef ORIGINAL_CODE
	std::vector<TextInputBox*> m_textInputs;
	int m_yOffset;
#endif

	bool m_bLastPointerPressedState;
	bool m_bRenderPointer;

	unsigned int m_lastTimeMoved;
	unsigned int m_cursorTick;
	std::map<GameController::StickID, GameController::StickState> m_lastStickState;
};

