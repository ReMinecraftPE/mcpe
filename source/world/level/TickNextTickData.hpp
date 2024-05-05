/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <stdint.h>

struct TickNextTickData
{
	static int C;

	TickNextTickData(int a, int b, int c, int d);
	int hashCode() const;
	bool operator<(const TickNextTickData& other) const;
	bool operator==(const TickNextTickData& other) const;
	void setDelay(int32_t);

	int m_ID;
	int field_4;
	int field_8;
	int field_C;
	int field_10;
	int32_t m_delay;
};

