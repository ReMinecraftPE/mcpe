/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TickNextTickData.hpp"

int TickNextTickData::C;

TickNextTickData::TickNextTickData(int a, int b, int c, int d)
{
	m_ID = ++C; //@NOTE: not C++
	field_4 = a;
	field_8 = b;
	field_C = c;
	field_10 = d;

#ifndef ORIGINAL_CODE
	m_delay = 0;
#endif
}

int TickNextTickData::hashCode() const
{
	return field_10 + ((field_8 + ((field_C + (field_4 << 10)) << 7)) << 8);
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
		field_8  == other.field_8  &&
		field_C  == other.field_C  &&
		field_10 == other.field_10;
}

void TickNextTickData::setDelay(TLong l)
{
	m_delay = l;
}
