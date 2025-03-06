/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "MouseDevice.hpp"

class ITouchScreenModel
{
public:
	virtual ~ITouchScreenModel();
	virtual int getPointerId(const MouseAction&) const;
	virtual int getPointerId(int x, int y, int pointerId) const;
};

