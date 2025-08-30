/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "compat/LegacyCPP.hpp"

class IArea
{
public:
	IArea() : field_4(true) {}
	virtual ~IArea() {}
	virtual bool isInside(float, float) = 0;

public:
	bool field_4; // could this mean something like "was allocated via new()"?
};

