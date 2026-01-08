/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <stddef.h>
#include "Multitouch.hpp"

int Multitouch::_activePointerCount;
int Multitouch::_activePointerList[MAX_TOUCHES];
int Multitouch::_index = -1;
bool Multitouch::_wasPressed[MAX_TOUCHES];
bool Multitouch::_wasReleased[MAX_TOUCHES];
bool Multitouch::_wasPressedThisUpdate[MAX_TOUCHES];
bool Multitouch::_wasReleasedThisUpdate[MAX_TOUCHES];
MouseDevice Multitouch::_pointers[MAX_TOUCHES];
std::vector<MouseAction> Multitouch::_inputs;

int Multitouch::_clampPointerId(int Id)
{
	if (Id < 0)
		return Id; //! @BUG

	if (Id >= MAX_TOUCHES)
		return MAX_TOUCHES - 1;

	return Id;
}

void Multitouch::commit()
{
	_activePointerCount = 0;
	for (int i = 0; i < MAX_TOUCHES; i++)
	{
		if (_pointers[i].isButtonDown(BUTTON_LEFT))
		{
			_activePointerList[_activePointerCount++] = i;
		}
	}
}

void Multitouch::feed(MouseButtonType a1, bool a2, int a3, int a4, int fingerId)
{
	fingerId = _clampPointerId(fingerId);
	MouseAction action(a1, a2, a3, a4, fingerId);
	_inputs.push_back(action);

	MouseDevice* pDevice = g(fingerId);
	pDevice->feed(a1, a2, a3, a4);

	if (a1 != BUTTON_NONE)
	{
		if (a2)
		{
			_wasPressed[fingerId] = true;
			_wasPressedThisUpdate[fingerId] = true;
		}
		else
		{
			_wasReleased[fingerId] = true;
			_wasReleasedThisUpdate[fingerId] = true;
		}
	}
}

MouseDevice* Multitouch::g(int Index)
{
	return &_pointers[_clampPointerId(Index)];
}

int Multitouch::getActivePointerIds(const int** out)
{
	*out = _activePointerList;
	return _activePointerCount;
}

MouseAction* Multitouch::getEvent()
{
	return &_inputs[_index];
}

int Multitouch::getFirstActivePointerIdEx()
{
	for (int i = 0; i < MAX_TOUCHES; i++)
	{
		if (_pointers[i].isButtonDown(BUTTON_LEFT))
			return i;
	}
	
	for (int i = 0; i < MAX_TOUCHES; i++)
	{
		if (_wasReleased[i])
			return i;
	}

	return -1;
}

int Multitouch::getFirstActivePointerIdExThisUpdate()
{
	for (int i = 0; i < MAX_TOUCHES; i++)
	{
		if (_pointers[i].isButtonDown(BUTTON_LEFT))
			return i;
	}
	
	for (int i = 0; i < MAX_TOUCHES; i++)
	{
		if (_wasReleasedThisUpdate[i])
			return i;
	}

	return -1;
}

int Multitouch::getX(int fingerId)
{
	return g(fingerId)->getX();
}

int Multitouch::getY(int fingerId)
{
	return g(fingerId)->getY();
}

bool Multitouch::isPressed(int fingerId)
{
	return _wasPressed[_clampPointerId(fingerId)];
}

bool Multitouch::next()
{
	if ((size_t)_index + 1 >= _inputs.size())
		return false;

	_index++;
	return true;
}

void Multitouch::reset()
{
	_inputs.clear();
	_index = -1;
	
	for (int i = 0; i < MAX_TOUCHES; i++)
	{
		g(i)->reset();
		_wasPressed[i] = 0;
		_wasReleased[i] = 0;
	}
}

void Multitouch::resetThisUpdate()
{
	for (int i = 0; i < MAX_TOUCHES; i++)
	{
		_wasPressedThisUpdate[i] = 0;
		_wasReleasedThisUpdate[i] = 0;
	}
}

void Multitouch::rewind()
{
	_index = -1;
}
