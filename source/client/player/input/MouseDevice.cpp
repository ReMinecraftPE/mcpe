/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "MouseDevice.hpp"

MouseDevice::MouseDevice()
{
	_index = -1;
	_x = 0;
	_y = 0;
	_xOld = 0;
	_yOld = 0;

	for (int i = 0; i < BUTTON_COUNT; i++)
		_buttonStates[i] = false;
}

void MouseDevice::feed(MouseButtonType buttonType, bool buttonState, int posX, int posY)
{
	if (buttonType != BUTTON_NONE)
		_inputs.push_back(MouseAction(buttonType, buttonState, posX, posY, 0));

	// Make sure button type is valid
	if (buttonType < BUTTON_COUNT)
	{
		// Check if we're processing a button-state update
		if (buttonType != BUTTON_NONE)
			_buttonStates[buttonType] = buttonState;

		_xOld = _x;
		_yOld = _y;
		_x = posX;
		_y = posY;
	}
}

short MouseDevice::getX()
{
	return short(_x);
}

short MouseDevice::getY()
{
	return short(_y);
}

bool MouseDevice::next()
{
	if (_index + 1 >= _inputs.size())
		return false;

	_index++;
	return true;
}

MouseButtonType MouseDevice::getEventButton()
{
	return _inputs[_index]._buttonType;
}

bool MouseDevice::isButtonDown(MouseButtonType btn)
{
	return getButtonState(btn);
}

void MouseDevice::reset()
{
	_inputs.clear();
	_index = -1;
}

MouseAction* MouseDevice::getEvent()
{
	return &_inputs[_index];
}

bool MouseDevice::getButtonState(MouseButtonType btn)
{
	if (btn < BUTTON_MIN || btn >= BUTTON_COUNT)
		return false;

	return _buttonStates[btn];
}
/*
void MouseDevice::setX(int x)
{
	_x = x;
}

void MouseDevice::setY(int y)
{
	_y = y;
}
*/
void MouseDevice::reset2()
{
	_xOld = _x;
	_yOld = _y;
}

bool MouseDevice::getEventButtonState()
{
	return _inputs[_index]._buttonState;
}
