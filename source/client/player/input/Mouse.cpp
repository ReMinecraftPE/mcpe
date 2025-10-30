/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Mouse.hpp"

MouseDevice Mouse::_instance;

void Mouse::feed(MouseButtonType buttonType, bool buttonState, int posX, int posY)
{
	_instance.feed(buttonType, buttonState, posX, posY);
}

short Mouse::getX()
{
	return _instance.getX();
}

short Mouse::getY()
{
	return _instance.getY();
}

bool Mouse::next()
{
	return _instance.next();
}

bool Mouse::isButtonDown(MouseButtonType btn)
{
	return _instance.isButtonDown(btn);
}

bool Mouse::getButtonState(MouseButtonType btn)
{
	return _instance.getButtonState(btn);
}

bool Mouse::getEventButtonState()
{
	return _instance.getEventButtonState();
}

void Mouse::reset()
{
	_instance.reset();
}

void Mouse::reset2()
{
	_instance.reset2();
}

MouseButtonType Mouse::getEventButton()
{
	return _instance.getEventButton();
}

MouseAction* Mouse::getEvent()
{
	return _instance.getEvent();
}
