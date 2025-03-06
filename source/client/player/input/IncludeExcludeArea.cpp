/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "IncludeExcludeArea.hpp"

IncludeExcludeArea::IncludeExcludeArea()
{
}

IncludeExcludeArea::~IncludeExcludeArea()
{
	clear();
}

void IncludeExcludeArea::include(IArea* pArea)
{
	m_include.push_back(pArea);
}

void IncludeExcludeArea::exclude(IArea* pArea)
{
	m_exclude.push_back(pArea);
}

void IncludeExcludeArea::clear()
{
	if (field_4)
	{
		for (int i = 0; i < int(m_include.size()); i++)
		{
			IArea* area = m_include[i];

			// @BUG: Invalid access order! If m_include[i] is null (which,
			//       by the way, _really_ shouldn't be, then there will
			//       be a NULL dereference!
#ifdef ORIGINAL_CODE
			if (area->m_vertices && area)
#else
			if (area && area->field_4)
#endif
			{
				delete area;
			}
		}

		for (int i = 0; i < int(m_exclude.size()); i++)
		{
			IArea* area = m_exclude[i];

			// @BUG: Same as the include bug.
#ifdef ORIGINAL_CODE
			if (area->m_vertices && area)
#else
			if (area && area->field_4)
#endif
			{
				delete area;
			}
		}
	}

	m_include.clear();
	m_exclude.clear();
}

bool IncludeExcludeArea::isInside(float x, float y)
{
	// @NOTE: This could be done a lot easier. Here's a way:
	// for (each area in m_exclude)
	//     if (area.isInside(x,y))
	//         return false;
	// for (each area in m_include)
	//     if (area.isInside(x,y))
	//         return true;
	// return false;

	for (int i = 0; i < int(m_include.size()); i++)
	{
		IArea* includeArea = m_include[i];
		if (!includeArea->isInside(x, y))
			continue;

		bool good = true;
		for (int j = 0; j < int(m_exclude.size()); j++)
		{
			IArea* excludeArea = m_exclude[j];
			if (excludeArea->isInside(x, y))
			{
				good = false;
				break;
			}
		}

		if (good)
			return true;
	}

	return false;
}
