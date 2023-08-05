/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LightUpdate.hpp"
#include "world/level/Level.hpp"

void LightUpdate::update(Level* pLevel)
{
	int newBr, oldBr, newBrN, x2, x6, x7, x14, x13, x10, v24, x21, x17_1, v27, x9, x10_1, x8, x7_1, x3, x4, x1, x20;
	int x19, x18, x17, x16, x5, x1_1;
	bool x11;

	if ((m_z2 - m_z1 + 1) * (m_x2 + 1 - m_x1 + (m_y2 - m_y1) * (m_x2 + 1 - m_x1)) > 32768)
		return;

	if (m_x2 < m_x1)
		return;

	x1 = m_x1 + 1;
	for (int i = m_x1 - 1; ; ++i)
	{
		x2 = x1 - 1;
		if (m_z2 < m_z1)
		{
			x1_1 = x1;
			goto LABEL_53;
		}
		x1_1 = x1;
		x3 = m_z1 + 1;
		x4 = m_z1 - 1;
		do
		{
			x6 = x3 - 1;
			if (!pLevel->hasChunksAt(x2, 0, x3 - 1, 1)
				|| pLevel->getChunk(x2 >> 4, x6 >> 4)->isEmpty())
			{
				x5 = x3;
			}
			else
			{
				if (m_y1 < 0)   m_y1 = 0;
				if (m_y2 > 127) m_y2 = 127;

				if (m_y1 <= m_y2)
				{
					x7 = m_y1 + 1;
					x8 = m_y1 - 1;
					x5 = x3;
					while (1)
					{
						oldBr = pLevel->getBrightness(*this->m_lightLayer, x2, x7 - 1, x6);
						x13 = pLevel->getTile(x2, x7 - 1, x6);
						x14 = Tile::lightBlock[x13];
						if (!x14)
							x14 = 1;
						if (m_lightLayer == &LightLayer::Sky)
							break;
						if (m_lightLayer != &LightLayer::Block)
							goto LABEL_30;
						x10 = Tile::lightEmission[x13];
						x11 = x10 == 0;
					LABEL_31:
						if (x14 > 14)
							v24 = x11;
						else
							v24 = 0;
						if (!v24)
						{
						LABEL_35:
							x10_1 = x10;
							x16 = pLevel->getBrightness(*m_lightLayer, i, x7 - 1, x6);
							x17 = pLevel->getBrightness(*m_lightLayer, x1, x7 - 1, x6);
							x7_1 = x7;
							x18 = pLevel->getBrightness(*m_lightLayer, x2, x8, x6);
							x19 = pLevel->getBrightness(*m_lightLayer, x2, x7, x6);
							x20 = pLevel->getBrightness(*m_lightLayer, x2, x7 - 1, x4);
							x21 = pLevel->getBrightness(*m_lightLayer, x2, x7 - 1, x3);
							x17_1 = x17;
							if (x17 < x16)
								x17_1 = x16;
							if (x17_1 < x18)
								x17_1 = x18;
							if (x17_1 < x19)
								x17_1 = x19;
							if (x17_1 < x20)
								x17_1 = x20;
							if (x17_1 < x21)
								v27 = x21 - x14;
							else
								v27 = x17_1 - x14;
							newBr = v27;
							if (newBr < 0)
								newBr = 0;
							if (newBr < x10_1)
								newBr = x10_1;
							goto LABEL_18;
						}
						newBr = 0;
						x7_1 = x7;
					LABEL_18:
						if (newBr != oldBr)
						{
							pLevel->setBrightness(*m_lightLayer, x2, x7 - 1, x6, newBr);
							newBrN = newBr - 1;
							if (newBrN < 0)
								newBrN = 0;
							pLevel->updateLightIfOtherThan(*m_lightLayer, i, x7 - 1, x6, newBrN);
							pLevel->updateLightIfOtherThan(*m_lightLayer, x2, x8, x6, newBrN);
							pLevel->updateLightIfOtherThan(*m_lightLayer, x2, x7 - 1, x4, newBrN);
							if (m_x2 <= x1)
								pLevel->updateLightIfOtherThan(*m_lightLayer, x1, x7 - 1, x6, newBrN);
							if (m_y2 <= x7)
								pLevel->updateLightIfOtherThan(*m_lightLayer, x2, x7, x6, newBrN);
							if (m_z2 <= x3)
								pLevel->updateLightIfOtherThan(*m_lightLayer, x2, x7 - 1, x3, newBrN);
						}
						++x7;
						++x8;
						if (m_y2 < x7_1)
							goto LABEL_8;
					}
					x9 = pLevel->isSkyLit(x2, x7 - 1, x6);
					x10 = 15;
					if (x9)
						goto LABEL_35;
				LABEL_30:
					x11 = 1;
					x10 = 0;
					goto LABEL_31;
				}
				x5 = x3;
			}
		LABEL_8:
			m_z2 = this->m_z2;
			++x3;
			++x4;
		} while (x5 <= m_z2);
		m_x2 = this->m_x2;
	LABEL_53:
		++x1;
		if (x1_1 > m_x2)
			break;
		m_z1 = this->m_z1;
	}
}

bool LightUpdate::expandToContain(int x1, int y1, int z1, int x2, int y2, int z2)
{
	if (m_x1 <= x1 &&
		m_y1 <= y1 &&
		m_z1 <= z1 &&
		m_x2 >= x2 &&
		m_y2 >= y2 &&
		m_z2 >= z2)
		return true;

	if (x1 < m_x1 - 1)  return false;
	if (y1 < m_y1 - 1)  return false;
	if (z1 < m_z1 - 1)  return false;
	if (x2 > m_x2 + 1) return false;
	if (y2 > m_y2 + 1) return false;
	if (z2 > m_z2 + 1) return false;

	if (y1 >= m_y1) y1 = m_y1;
	if (x1 >= m_x1) x1 = m_x1;
	if (y2 < m_y2) y2 = m_y2;
	if (z1 >= m_z1) z1 = m_z1;
	if (x2 < m_x2) x2 = m_x2;
	if (z2 < m_z2) z2 = m_z2;

	// If trying to add more than 2 tiles, we can't do that
	if ((z2 - z1) * (x2 - x1) * (y2 - y1) - (m_z2 - m_z1) * (m_x2 - m_x1) * (m_y2 - m_y1) > 2)
		return false;

	m_x1 = x1;
	m_y1 = y1;
	m_x2 = x2;
	m_y2 = y2;
	m_z1 = z1;
	m_z2 = z2;

	return true;
}
