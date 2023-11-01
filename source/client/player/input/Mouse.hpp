/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <vector>

#include "MouseDevice.hpp"

struct MouseAction;

class Mouse
{
public:
	static void  feed(MouseButtonType buttonType, bool buttonState, int posX, int posY);
	static short getX();
	static short getY();
	static bool  next();
	static bool  isButtonDown(MouseButtonType btn);
	static bool  getButtonState(MouseButtonType btn);
	static bool  getEventButtonState();
	//static void  setX(int x);
	//static void  setY(int y);
	static void  reset();
	static void  reset2();
	static MouseButtonType getEventButton();
	static MouseAction* getEvent();

private:
	static MouseDevice _instance;
};
