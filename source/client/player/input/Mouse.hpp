/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <vector>

#define MOUSE_STATES_SIZE (Mouse::ButtonState::MAX + 1)

struct MouseAction;

class Mouse
{
public:
	enum ButtonType
	{
		NONE,
		LEFT,
		RIGHT,
		MIDDLE,
		_MIN = LEFT,
		_MAX = MIDDLE
	};

	enum ButtonState : bool
	{
		UP,
		DOWN,
		MAX = DOWN // God yes, I love C++. It doesn't let me name this _MAX despite it being in a DIFFERENT ENUM ENTIRELY...
	};

	static void feed(ButtonType buttonType, ButtonState buttonState, int posX, int posY);

	static short getX();
	static short getY();
	static bool  next();
	static bool  isButtonDown(int btn);
	static ButtonState getButtonState(ButtonType btn);
	static ButtonType getEventButton();
	static ButtonState getEventButtonState();
	static MouseAction* getEvent();
	static void  setX(int x);
	static void  setY(int y);
	static void  reset();
	static void  reset2();

	// @TODO: There's plenty of inlined code here. Out-line it.

private:
	static std::vector<MouseAction> _inputs;
	static int _index;
	static int _x, _y;
	static int _xOld, _yOld;
	static ButtonState _buttonStates[MOUSE_STATES_SIZE];
};

struct MouseAction
{
	Mouse::ButtonType _buttonType;
	Mouse::ButtonState _buttonState;
	int _posX;
	int _posY;

	MouseAction()
	{
		_buttonType = Mouse::ButtonType::NONE;
		_buttonState = Mouse::ButtonState::DOWN;
		_posX = 0;
		_posY = 0;
	}

	MouseAction(Mouse::ButtonType buttonType, Mouse::ButtonState buttonState, int posX, int posY)
	{
		_buttonType = buttonType;
		_buttonState = buttonState;
		_posX = posX;
		_posY = posY;
	}

	bool isButton()
	{
		return _buttonType == Mouse::ButtonType::LEFT ||
			_buttonType == Mouse::ButtonType::RIGHT ||
			_buttonType == Mouse::ButtonType::MIDDLE;
	}
};
