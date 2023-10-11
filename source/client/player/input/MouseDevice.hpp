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
	BUTTON_NONE,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_MIDDLE,
	BUTTON_COUNT,
	BUTTON_SCROLLWHEEL,
	BUTTON_MIN = BUTTON_LEFT,
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
		_buttonType = BUTTON_NONE;
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
			_buttonType == BUTTON_LEFT ||
			_buttonType == BUTTON_RIGHT ||
			_buttonType == BUTTON_MIDDLE;
	}
};

class MouseDevice
{
public:
	MouseDevice()
	{
		_index = -1;
	}

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
	bool _buttonStates[BUTTON_COUNT];
	std::vector<MouseAction> _inputs;
};

