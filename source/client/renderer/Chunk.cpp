/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Chunk.hpp"
#include "renderer/RenderContextImmediate.hpp"
#include "world/level/Level.hpp"
#include "world/level/Region.hpp"
#include "TileRenderer.hpp"

int Chunk::updates;

float Chunk::distanceToSqr(const Entity& entity) const
{
	float dX = entity.m_pos.x - float(m_posM.x);
	float dY = entity.m_pos.y - float(m_posM.y);
	float dZ = entity.m_pos.z - float(m_posM.z);
	return dX * dX + dY * dY + dZ * dZ;
}
float Chunk::squishedDistanceToSqr(const Entity& entity) const
{
	float dX = entity.m_pos.x - float(m_posM.x);
	float dY = entity.m_pos.y - float(m_posM.y);
	float dZ = entity.m_pos.z - float(m_posM.z);

	dY *= 2;

	return dX * dX + dY * dY + dZ * dZ;
}

void Chunk::reset()
{
	for (int i = Tile::RENDER_LAYERS_MIN; i <= Tile::RENDER_LAYERS_MAX; i++)
	{
		m_empty[i] = true;
	}
	m_bVisible = false;
	m_bCompiled = false;
}

int Chunk::getList(Tile::RenderLayer layer)
{
	if (!m_bVisible)
		return -1;

	if (m_empty[layer])
		return -1;

	return m_lists + layer;
}

RenderChunk* Chunk::getRenderChunk(Tile::RenderLayer layer)
{
	return &m_renderChunks[layer];
}

int Chunk::getAllLists(int* displayLists, int p, Tile::RenderLayer layer)
{
	if (!m_bVisible)
		return p;

	if (m_empty[layer])
		return p;

	displayLists[p++] = m_lists + layer;

	return p;
}

void Chunk::cull(Culler* pCuller)
{
	m_bVisible = pCuller->isVisible(m_aabb);
}

void Chunk::renderBB()
{
	//glCallList(m_lists + 2);
}

bool Chunk::isEmpty()
{
	if (!m_bCompiled)
		return false;

	for (int i = Tile::RENDER_LAYERS_MIN; i <= Tile::RENDER_LAYERS_MAX; i++)
	{
		if (!m_empty[i])
			return false;
	}
	
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
	m_posM = pos + m_posS / 2;

	m_aabb = AABB(m_pos - 1, m_pos + m_posS + 1);

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

	for (int i = Tile::RENDER_LAYERS_MIN; i <= Tile::RENDER_LAYERS_MAX; i++)
	{
		m_empty[i] = true;
	}

	TilePos min(m_pos), max(m_pos + m_posS);
	Region region(m_pLevel, min - 1, max + 1);

	mce::RenderContext& renderContext = mce::RenderContextImmediate::get();
	Tesselator& t = Tesselator::instance;
	TileRenderer tileRenderer(t, &region);

	TilePos tp(min);
	for (int layer = Tile::RENDER_LAYERS_MIN; layer <= Tile::RENDER_LAYERS_MAX; layer++)
	{
		bool started = false, rendered = false, renderNextLayer = false;
		for (tp.y = min.y; tp.y < max.y; tp.y++)
		{
			for (tp.z = min.z; tp.z < max.z; tp.z++)
			{
				for (tp.x = min.x; tp.x < max.x; tp.x++)
				{
					TileID tile = region.getTile(tp);
					if (tile <= 0) continue;

					if (!started)
					{
						started = true;
						if (tileRenderer.useAmbientOcclusion())
						{
							renderContext.setShadeMode(mce::SHADE_MODE_SMOOTH);
						}
						t.begin(12000);
						t.setOffset(-m_pos);
					}

					Tile* pTile = Tile::tiles[tile];

					if (layer == pTile->getRenderLayer())
					{
						if (tileRenderer.tesselateInWorld(pTile, tp))
							rendered = true;
					}
					else
					{
						renderNextLayer = true;
					}
				}
			}
		}

		if (started)
		{
			mce::Mesh chunkMesh = t.end();
			RenderChunk* pRChk = &m_renderChunks[layer];

			*pRChk = RenderChunk(m_pos, chunkMesh);

			t.setOffset(Vec3::ZERO);

			if (rendered)
				m_empty[layer] = false;
		}

		if (!renderNextLayer)
			break;
	}

	field_54 = LevelChunk::touchedSky;
	m_bCompiled = true;
}

Chunk::Chunk(Level* level, const TilePos& pos, int size, int lists)
{
	m_bOcclusionVisible = true;
	m_bOcclusionQuerying = false;
	m_bCompiled = false;
	m_bDirty = false;

	m_pLevel = level;
	m_posS = TilePos(size, size, size);
	m_pTesselator = &Tesselator::instance;
	m_lists = lists;
	m_pos.x = -999;
	field_2C = Vec3(pos).lengthSqr() / 2;

	reset();
	setPos(pos);
}
