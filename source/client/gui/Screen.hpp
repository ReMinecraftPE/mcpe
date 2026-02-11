/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <vector>
#include <map>

#include "renderer/MaterialPtr.hpp"
#include "client/app/Minecraft.hpp"
#include "client/player/input/Mouse.hpp"
#include "client/player/input/Keyboard.hpp"
#include "client/player/input/GameController.hpp"
#include "GuiElement.hpp"
#include "MenuPointer.hpp"
#include "AreaNavigation.hpp"

class Button;
class VerticalLayout;

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
	void _addElement(GuiElement& element, bool navigable = true);
	void _selectCurrentElement();
	void _deselectCurrentElement();
	void _playSelectSound();
	void _renderPointer();
	GuiElement* _getElement(GuiElement::ID id); // returns any element
	GuiElement* _getSelectedElement();
	bool _useController() const;

public:
	void init(Minecraft*, int, int);
	void setSize(int width, int height);
	void onRender(float f);
	bool onBack(bool b);
	bool selectElement(AreaNavigation::ID id);
	void selectElement(GuiElement*);
	bool nextTab();
	bool prevTab();
	int getYOffset();
	unsigned int getCursorMoveThrottle() const { return 65; }
	bool doElementTabbing() const;
	void controllerEvent(GameController::StickID stickId, double deltaTime = 0.0);

protected:
	virtual bool _areaNavigation(AreaNavigation::Direction);
	virtual void _processControllerDirection(GameController::StickID stickId);
	virtual void _controllerDirectionChanged(GameController::StickID stickId, GameController::StickState stickState);
	virtual void _controllerDirectionHeld(GameController::StickID stickId, GameController::StickState stickState);
	virtual void _buttonClicked(Button* pButton);
	virtual void _guiElementClicked(GuiElement& element);
	virtual void _updateTabButtonSelection();
	virtual bool _nextTab();
	virtual bool _prevTab();

public:
	virtual void render(float a);
	virtual void init() {};
	virtual void initMenuPointer();
	virtual void updateEvents();
	virtual void mouseEvent();
	virtual void keyboardEvent();
	virtual void controllerEvent();
	virtual void checkForPointerEvent(MouseButtonType button);
	virtual bool handleBackEvent(bool b);
	virtual void onClose();
	virtual void handlePointerLocation(MenuPointer::Unit x, MenuPointer::Unit y);
	virtual void handlePointerPressed(bool isPressed);
	virtual void handlePointerAction(const MenuPointer& pointer, MouseButtonType button);
	virtual void handleScrollWheel(float force);
	virtual void handleControllerStickEvent(const GameController::StickEvent& stick, double deltaTime);
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
	virtual void pointerPressed(const MenuPointer& pointer, MouseButtonType btn);
	virtual void pointerReleased(const MenuPointer& pointer, MouseButtonType btn);
	virtual void keyPressed(int);
	virtual void handleTextChar(char);
	virtual void keyboardTextPaste(const std::string& text);
	virtual float getScale(int width, int height);
	static float getConsoleScale(int height);

	// ported from 0.8
	virtual void renderMenuBackground(float f);
	void renderConsolePanorama(bool isNight);
	void renderConsolePanorama();
	void renderConsoleLoading(int x, int y, int blockSize = 42, int blockDistance = 12);

protected:
	Materials m_screenMaterials;
	MenuPointer m_menuPointer;
	bool m_bLastPointerPressedState;

public:
	friend class VerticalLayout;

	class Navigation : public AreaNavigation
	{
	public:
		Navigation(Screen*);

		bool next(int& x, int& y, bool invert) override;

		bool isValid(ID) override;

	private:
		Screen* m_pScreen;
	};

	int m_width;
	int m_height;
	bool m_bPassEvents;
	//@NOTE: This should be enabled only if the the actual screen handles the deletion of the previous screen, otherwise, there will be a memory leak!
	bool m_bDeletePrevious;
	Minecraft* m_pMinecraft;
	GuiElementList m_elements;
	GuiElement* m_pSelectedElement;
	Font* m_pFont;
	GuiElement* m_pClickedElement;
	UITheme m_uiTheme;

#ifndef ORIGINAL_CODE
	int m_yOffset;
#endif

	bool m_bRenderPointer;

	unsigned int m_lastTimeMoved;
	unsigned int m_cursorTick;
	std::map<GameController::StickID, GameController::StickState> m_lastStickState;
};

