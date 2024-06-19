/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TickNextTickData.hpp"

int TickNextTickData::C;

TickNextTickData::TickNextTickData(const TilePos& tilePos, int d)
{
	m_ID = ++C; //@NOTE: not C++
	field_4 = tilePos;
	field_10 = d;

#ifndef ORIGINAL_CODE
	m_delay = 0;
#endif
}

int TickNextTickData::hashCode() const
{
	return field_10 + ((field_4.y + ((field_4.z + (field_4.x << 10)) << 7)) << 8);
}

bool TickNextTickData::operator<(const TickNextTickData& other) const
{
	if (m_delay < other.m_delay)
		return true;
	if (m_delay > other.m_delay)
		return false;

	return m_ID < other.m_ID;
}

bool TickNextTickData::operator==(const TickNextTickData& other) const
{
	return
		field_4  == other.field_4  &&
		field_10 == other.field_10;
}

void TickNextTickData::setDelay(int32_t l)
{
	m_delay = l;
}
