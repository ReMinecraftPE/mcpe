/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <vector>

enum MouseButtonType
{
	MOUSE_BUTTON_NONE,
	MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_RIGHT,
	MOUSE_BUTTON_MIDDLE,
	MOUSE_BUTTON_COUNT,
	MOUSE_BUTTON_SCROLLWHEEL,

	MOUSE_BUTTON_MIN = MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_MAX = MOUSE_BUTTON_SCROLLWHEEL
};

struct MouseAction
{
	MouseButtonType _buttonType;
	bool _buttonState;
	int _posX;
	int _posY;
	int _fingerId;

	MouseAction()
	{
		_buttonType = MOUSE_BUTTON_NONE;
		_buttonState = 0;
		_posX = 0;
		_posY = 0;
		_fingerId = 0;
	}

	MouseAction(MouseButtonType buttonType, bool buttonState, int posX, int posY, int fingerId)
	{
		_buttonType = buttonType;
		_buttonState = buttonState;
		_posX = posX;
		_posY = posY;
		_fingerId = fingerId;
	}

	bool isButton()
	{
		return
			_buttonType == MOUSE_BUTTON_LEFT ||
			_buttonType == MOUSE_BUTTON_RIGHT ||
			_buttonType == MOUSE_BUTTON_MIDDLE;
	}
};

class MouseDevice
{
public:
	MouseDevice();
	void  feed(MouseButtonType buttonType, bool buttonState, int posX, int posY);
	short getX();
	short getY();
	bool  next();
	bool  isButtonDown(MouseButtonType btn);
	bool  getButtonState(MouseButtonType btn);
	bool  getEventButtonState();
	//void  setX(int x);
	//void  setY(int y);
	void  reset();
	void  reset2();
	MouseButtonType getEventButton();
	MouseAction* getEvent();

private:
	int _index;
	int _x, _y;
	int _xOld, _yOld;
	bool _buttonStates[MOUSE_BUTTON_COUNT];
	std::vector<MouseAction> _inputs;
};

