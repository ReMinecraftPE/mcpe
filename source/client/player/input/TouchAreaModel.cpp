/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TouchAreaModel.hpp"

TouchAreaModel::~TouchAreaModel()
{
	clear();
}

int TouchAreaModel::getPointerId(const MouseAction& action)
{
	return getPointerId(action._posX, action._posY, action._fingerId);
}

int TouchAreaModel::getPointerId(int x, int y, int pointerId)
{
	// Yes, it explicitly uses .size() in the original
	for (size_t i = 0; i < m_areas.size(); i++)
	{
		Area* pArea = m_areas[i];
		if (pArea->m_pArea->isInside(float(x), float(y)))
			return pArea->m_id;
	}

	return pointerId;
}

void TouchAreaModel::clear()
{
	for (size_t i = 0; i < m_areas.size(); i++)
		delete m_areas[i];

	m_areas.clear();
}

void TouchAreaModel::addArea(int id, IArea* pArea)
{
	Area* pTArea = new Area();

	// NOTE: The original code first initializes the
	// fields of the new Area instance to zero, and
	// then sets them to these values.
	// So we do the same here.
	pTArea->m_pArea = pArea;
	pTArea->m_id = id;

	m_areas.push_back(pTArea);
}
