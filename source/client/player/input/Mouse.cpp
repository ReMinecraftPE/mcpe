/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Mouse.hpp"

std::vector<MouseAction> Mouse::_inputs;
int Mouse::_index, Mouse::_x, Mouse::_y;
int Mouse::_xOld, Mouse::_yOld;
Mouse::ButtonState Mouse::_buttonStates[Mouse::ButtonType::COUNT];

void Mouse::feed(ButtonType buttonType, ButtonState buttonState, int posX, int posY)
{
	if (buttonType != ButtonType::NONE)
		_inputs.push_back(MouseAction(buttonType, buttonState, posX, posY));

	// Make sure button type is valid
	if (buttonType <= ButtonType::COUNT)
	{
		// Check if we're processing a button-state update
		if (buttonType != ButtonType::NONE)
			_buttonStates[buttonType] = buttonState;

		_xOld = _x;
		_yOld = _y;
		_x = posX;
		_y = posY;
	}
}

short Mouse::getX()
{
	return short(_x);
}

short Mouse::getY()
{
	return short(_y);
}

bool Mouse::next()
{
	if (_index + 1 >= _inputs.size())
		return false;

	_index++;
	return true;
}

Mouse::ButtonType Mouse::getEventButton()
{
	return _inputs[_index]._buttonType;
}

bool Mouse::isButtonDown(int btn)
{
	return _buttonStates[btn];
}

void Mouse::reset()
{
	_inputs.clear();
	_index = -1;
}

MouseAction* Mouse::getEvent()
{
	return &_inputs[_index];
}

Mouse::ButtonState Mouse::getButtonState(ButtonType btn)
{
	if (btn <= ButtonType::MIN || btn > ButtonType::COUNT)
		return ButtonState::UP;

	return _buttonStates[btn];
}

void Mouse::setX(int x)
{
	_x = x;
}

void Mouse::setY(int y)
{
	_y = y;
}

void Mouse::reset2()
{
	_xOld = _x;
	_yOld = _y;
}

Mouse::ButtonState Mouse::getEventButtonState()
{
	return _inputs[_index]._buttonState;
}
