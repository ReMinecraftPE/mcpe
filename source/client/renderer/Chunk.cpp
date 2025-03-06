/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Chunk.hpp"
#include "world/level/Level.hpp"
#include "world/level/Region.hpp"
#include "TileRenderer.hpp"

int Chunk::updates;

float Chunk::distanceToSqr(const Entity* pEnt) const
{
	float dX = pEnt->m_pos.x - float(m_pos2.x);
	float dY = pEnt->m_pos.y - float(m_pos2.y);
	float dZ = pEnt->m_pos.z - float(m_pos2.z);
	return dX * dX + dY * dY + dZ * dZ;
}
float Chunk::squishedDistanceToSqr(const Entity* pEnt) const
{
	float dX = pEnt->m_pos.x - float(m_pos2.x);
	float dY = pEnt->m_pos.y - float(m_pos2.y);
	float dZ = pEnt->m_pos.z - float(m_pos2.z);

	dY *= 2;

	return dX * dX + dY * dY + dZ * dZ;
}

void Chunk::reset()
{
	field_1C[0] = true;
	field_1C[1] = true;
	m_bVisible = false;
	field_94 = false;
}

int Chunk::getList(int idx)
{
	if (!m_bVisible)
		return -1;

	if (field_1C[idx])
		return -1;

	return field_8C + idx;
}

RenderChunk* Chunk::getRenderChunk(int idx)
{
	return &m_renderChunks[idx];
}

int Chunk::getAllLists(int* arr, int arr_idx, int idx)
{
	if (!m_bVisible)
		return arr_idx;

	if (field_1C[idx])
		return arr_idx;

	arr[arr_idx++] = field_8C + idx;

	return arr_idx;
}

void Chunk::cull(Culler* pCuller)
{
	m_bVisible = pCuller->isVisible(m_aabb);
}

void Chunk::renderBB()
{

}

bool Chunk::isEmpty()
{
	if (!field_94)
		return false;

	if (!field_1C[0])
		return false;

	if (!field_1C[1])
		return false;

	return true;
}

void Chunk::setDirty()
{
	m_bDirty = true;
}

void Chunk::setPos(const TilePos& pos)
{
	if (m_pos == pos)
		// No change.
		return;

	m_pos = pos;
	m_pos2 = pos + field_10 / 2;

	m_aabb = AABB(m_pos - 1, m_pos + field_10 + 1);

	setDirty();
}

void Chunk::setClean()
{
	m_bDirty = false;
}

bool Chunk::isDirty()
{
	return m_bDirty;
}

void Chunk::rebuild()
{
	if (!m_bDirty)
		return;

	updates++;

	LevelChunk::touchedSky = false;

	field_1C[0] = true;
	field_1C[1] = true;

	TilePos min(m_pos), max(m_pos + field_10);

	Region region(m_pLevel, min - 1, max + 1);
	TileRenderer tileRenderer(&region);

	Tesselator& t = Tesselator::instance;

	TilePos tp(min);
	for (int layer = 0; layer < 2; layer++)
	{
		bool bTesselatedAnything = false, bDrewThisLayer = false, bNeedAnotherLayer = false;
		for (tp.y = min.y; tp.y < max.y; tp.y++)
		{
			for (tp.z = min.z; tp.z < max.z; tp.z++)
			{
				for (tp.x = min.x; tp.x < max.x; tp.x++)
				{
					TileID tile = region.getTile(tp);
					if (tile <= 0) continue;

					if (!bTesselatedAnything)
					{
						bTesselatedAnything = true;
						if (tileRenderer.useAmbientOcclusion())
							glShadeModel(GL_SMOOTH);
						t.begin();
						t.offset(float(-m_pos.x), float(-m_pos.y), float(-m_pos.z));
					}

					Tile* pTile = Tile::tiles[tile];

					if (layer == pTile->getRenderLayer())
					{
						if (tileRenderer.tesselateInWorld(pTile, tp))
							bDrewThisLayer = true;
					}
					else
					{
						bNeedAnotherLayer = true;
					}
				}
			}
		}

		if (bTesselatedAnything)
		{
			RenderChunk rchk = t.end(field_90[layer]);
			RenderChunk* pRChk = &m_renderChunks[layer];

			*pRChk = rchk;
			pRChk->field_C  = float(m_pos.x);
			pRChk->field_10 = float(m_pos.y);
			pRChk->field_14 = float(m_pos.z);

			t.offset(0.0f, 0.0f, 0.0f);

			if (bDrewThisLayer)
				field_1C[layer] = false;
		}

		if (!bNeedAnotherLayer)
			break;
	}

	field_54 = LevelChunk::touchedSky;
	field_94 = true;
}

void Chunk::translateToPos()
{
	glTranslatef(float(m_pos.x), float(m_pos.y), float(m_pos.z));
}

Chunk::Chunk(Level* level, const TilePos& pos, int a, int b, GLuint* bufs)
{
	field_4D = true;
	field_4E = false;
	field_94 = false;
	m_bDirty = false;

	m_pLevel = level;
	field_10 = TilePos(a, a, a);
	m_pTesselator = &Tesselator::instance;
	field_8C = b;
	m_pos.x = -999;
	field_2C = Vec3(pos).lengthSqr() / 2;
	field_90 = bufs;

	setPos(pos);
}
