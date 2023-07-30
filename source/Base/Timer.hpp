/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

class Timer
{
public:
	Timer();

	void advanceTime();

public:
	float field_0 = 0;
	int field_4 = 0;
	int field_8 = 0;
	float field_C = 1.0f;
	float field_10 = 20.0f;
	int field_14 = 0;
	float field_18 = 0;
	float field_1C = 1.0f;
	float field_20 = 0;
};

