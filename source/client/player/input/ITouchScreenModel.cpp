/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ITouchScreenModel.hpp"

ITouchScreenModel::~ITouchScreenModel()
{
}

int ITouchScreenModel::getPointerId(const MouseAction& action)
{
    return action._fingerId;
}

int ITouchScreenModel::getPointerId(int x, int y, int pointerId)
{
    return pointerId;
}
