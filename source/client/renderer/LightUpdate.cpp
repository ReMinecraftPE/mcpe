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
	int newBr, oldBr, newBrN, x, z, x7, x14, x13, x10, v24, x21, x17_1, v27, x9, x10_1, x8, x7_1, x3, x4, x1, x20;
	int x19, x18, x17, x16, x5, x1_1;
	bool x11;

	if ((m_tilePos2.z - m_tilePos1.z + 1) * (m_tilePos2.x + 1 - m_tilePos1.x + (m_tilePos2.y - m_tilePos1.y) * (m_tilePos2.x + 1 - m_tilePos1.x)) > 32768)
		return;

	if (m_tilePos2.x < m_tilePos1.x)
		return;

	x1 = m_tilePos1.x + 1;
	for (int i = m_tilePos1.x - 1; ; ++i)
	{
		x = x1 - 1;
		if (m_tilePos2.z < m_tilePos1.z)
		{
			x1_1 = x1;
			goto LABEL_53;
		}
		x1_1 = x1;
		x3 = m_tilePos1.z + 1;
		x4 = m_tilePos1.z - 1;
		do
		{
			z = x3 - 1;
			if (!pLevel->hasChunksAt(TilePos(x, 0, x3 - 1), 1)
				|| pLevel->getChunk(TilePos(x, 0, z))->isEmpty())
			{
				x5 = x3;
			}
			else
			{
				if (m_tilePos1.y < 0)   m_tilePos1.y = 0;
				if (m_tilePos2.y > 127) m_tilePos2.y = 127;

				if (m_tilePos1.y <= m_tilePos2.y)
				{
					x7 = m_tilePos1.y + 1;
					x8 = m_tilePos1.y - 1;
					x5 = x3;
					while (1)
					{
						oldBr = pLevel->getBrightness(*this->m_lightLayer, TilePos(x, x7 - 1, z));
						x13 = pLevel->getTile(TilePos(x, x7 - 1, z));
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
							x16 = pLevel->getBrightness(*m_lightLayer, TilePos(i, x7 - 1, z));
							x17 = pLevel->getBrightness(*m_lightLayer, TilePos(x1, x7 - 1, z));
							x7_1 = x7;
							x18 = pLevel->getBrightness(*m_lightLayer, TilePos(x, x8, z));
							x19 = pLevel->getBrightness(*m_lightLayer, TilePos(x, x7, z));
							x20 = pLevel->getBrightness(*m_lightLayer, TilePos(x, x7 - 1, x4));
							x21 = pLevel->getBrightness(*m_lightLayer, TilePos(x, x7 - 1, x3));
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
							pLevel->setBrightness(*m_lightLayer, TilePos(x, x7 - 1, z), newBr);
							newBrN = newBr - 1;
							if (newBrN < 0)
								newBrN = 0;
							pLevel->updateLightIfOtherThan(*m_lightLayer, TilePos(i, x7 - 1, z), newBrN);
							pLevel->updateLightIfOtherThan(*m_lightLayer, TilePos(x, x8, z), newBrN);
							pLevel->updateLightIfOtherThan(*m_lightLayer, TilePos(x, x7 - 1, x4), newBrN);
							if (m_tilePos2.x <= x1)
								pLevel->updateLightIfOtherThan(*m_lightLayer, TilePos(x1, x7 - 1, z), newBrN);
							if (m_tilePos2.y <= x7)
								pLevel->updateLightIfOtherThan(*m_lightLayer, TilePos(x, x7, z), newBrN);
							if (m_tilePos2.z <= x3)
								pLevel->updateLightIfOtherThan(*m_lightLayer, TilePos(x, x7 - 1, x3), newBrN);
						}
						++x7;
						++x8;
						if (m_tilePos2.y < x7_1)
							goto LABEL_8;
					}
					x9 = pLevel->isSkyLit(TilePos(x, x7 - 1, z));
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
			++x3;
			++x4;
		} while (x5 <= m_tilePos2.z);
	LABEL_53:
		++x1;
		if (x1_1 > m_tilePos2.x)
			break;
	}
}

bool LightUpdate::expandToContain(const TilePos& tilePos1, const TilePos& tilePos2)
{
	if (m_tilePos1 <= tilePos1 && m_tilePos2 >= tilePos2)
		return true;

	if (tilePos1 < m_tilePos1 - 1) return false;
	if (tilePos2 > m_tilePos2 + 1) return false;

	TilePos tp1(tilePos1), tp2(tilePos2);
	if (tp1.y >= m_tilePos1.y) tp1.y = m_tilePos1.y;
	if (tp1.x >= m_tilePos1.x) tp1.x = m_tilePos1.x;
	if (tp2.y < m_tilePos2.y)  tp2.y = m_tilePos2.y;
	if (tp1.z >= m_tilePos1.z) tp1.z = m_tilePos1.z;
	if (tp2.x < m_tilePos2.x)  tp2.x = m_tilePos2.x;
	if (tp2.z < m_tilePos2.z)  tp2.z = m_tilePos2.z;

	// If trying to add more than 2 tiles, we can't do that
	if ((tp2.z - tp1.z) * (tp2.x - tp1.x) * (tp2.y - tp1.y) - (m_tilePos2.z - m_tilePos1.z) * (m_tilePos2.x - m_tilePos1.x) * (m_tilePos2.y - m_tilePos1.y) > 2)
		return false;

	m_tilePos1 = tp1;
	m_tilePos2 = tp2;

	return true;
}
