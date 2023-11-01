/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

class IArea
{
public:
	IArea() : field_4(true) {}
	virtual bool isInside(float, float) = 0;

protected:
	friend class IncludeExcludeArea;

	bool field_4;
};

