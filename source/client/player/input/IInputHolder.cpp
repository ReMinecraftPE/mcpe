/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "IInputHolder.hpp"
#include "Mouse.hpp"

IInputHolder::IInputHolder() :
	field_4(0),
	field_8(0)
{
}

IInputHolder::~IInputHolder()
{
}

bool IInputHolder::allowPicking()
{
	field_4 = Mouse::getX();
	field_8 = Mouse::getY();
	return Mouse::getButtonState(BUTTON_LEFT);
}

void IInputHolder::setScreenSize(int width, int height)
{
	getMoveInput()->setScreenSize(width, height);
	getTurnInput()->setScreenSize(width, height);
	getBuildInput()->setScreenSize(width, height);
}
