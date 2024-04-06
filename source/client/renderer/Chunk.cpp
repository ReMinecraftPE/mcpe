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
#include "client/app/Minecraft.hpp"

#define ENABLE_GREEDY_MESHING (true)

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
	m_bIsEmptyLayer[0] = true;
	m_bIsEmptyLayer[1] = true;
	m_bVisible = false;
	field_94 = false;
}

int Chunk::getList(int idx)
{
	if (!m_bVisible)
		return -1;

	if (m_bIsEmptyLayer[idx])
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

	if (m_bIsEmptyLayer[idx])
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

	if (!m_bIsEmptyLayer[0])
		return false;

	if (!m_bIsEmptyLayer[1])
		return false;

	return true;
}

void Chunk::setDirty()
{
	m_bDirty = true;
}

void Chunk::setPos(int x, int y, int z)
{
	if (m_pos.x == x && m_pos.y == y && m_pos.z == z)
		// No change.
		return;

	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;
	m_pos2.x = x + field_10 / 2;
	m_pos2.y = y + field_14 / 2;
	m_pos2.z = z + field_18 / 2;

	m_aabb = AABB(float(m_pos.x - 1), float(m_pos.y - 1), float(m_pos.z - 1), float(m_pos.x + field_10 + 1), float(m_pos.y + field_14 + 1), float(m_pos.z + field_18 + 1));

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

void Chunk::renderXExpandableFaces(TileRenderer& tileRenderer, Region& region, int layer, int direction, bool& bNeedAnotherLayer)
{
	int minX = m_pos.x, maxX = m_pos.x + field_10;
	int minY = m_pos.y, maxY = m_pos.y + field_14;
	int minZ = m_pos.z, maxZ = m_pos.z + field_18;
	
	// Draw face down and non-solids
	for (int y = minY; y < maxY; y++)
		for (int z = minZ; z < maxZ; z++)
			for (int x = minX; x < maxX; x++)
			{
				TileID tile = region.getTile(x, y, z);
				if (tile == 0) continue;

				Tile* pTile = Tile::tiles[tile];
				int tshape = pTile->getRenderShape();
				int tlayer = pTile->getRenderLayer();

				if (layer != tlayer) {
					bNeedAnotherLayer = true;
					continue;
				}

				// Sadly, biome colored grass won't work
				if (!TileRenderer::canRenderGreedy(tshape) ||
					(tile == TILE_GRASS && tileRenderer.m_bBiomeColors))
				{
					if (direction == DIR_YNEG)
						tileRenderer.tesselateInWorld(pTile, x, y, z);

					continue;
				}

				int x2 = x;
				// Count the amount of tiles on +X that we are repeating for.
				while (x2 < m_pos.x + 16 && region.getTile(x2, y, z) == tile)
					x2++;

				// The group is x2 - x tiles wide. Render it
				tileRenderer.setGroupSize(x2 - x);
				if (tileRenderer.tesselateBlockInWorldDir(pTile, x, y, z, -1.0f, -1.0f, -1.0f, direction))
				{
					// Decrease x2 as we actually want the last tile within the group.
					x2--;
					// Skip all other tiles, as they were rendered in this iteration.
					x = x2;
				}
			}
}

void Chunk::renderZExpandableFaces(TileRenderer& tileRenderer, Region& region, int layer, int direction, bool& bNeedAnotherLayer)
{
	int minX = m_pos.x, maxX = m_pos.x + field_10;
	int minY = m_pos.y, maxY = m_pos.y + field_14;
	int minZ = m_pos.z, maxZ = m_pos.z + field_18;
	
	// Draw face down and non-solids
	for (int y = minY; y < maxY; y++)
		for (int x = minX; x < maxX; x++)
			for (int z = minZ; z < maxZ; z++)
			{
				TileID tile = region.getTile(x, y, z);
				if (tile == 0) continue;

				Tile* pTile = Tile::tiles[tile];
				int tshape = pTile->getRenderShape();
				int tlayer = pTile->getRenderLayer();

				if (layer != tlayer) {
					bNeedAnotherLayer = true;
					continue;
				}

				// Sadly, biome colored grass won't work
				if (!TileRenderer::canRenderGreedy(tshape) ||
					(tile == TILE_GRASS && tileRenderer.m_bBiomeColors))
					continue;

				int z2 = z;
				// Count the amount of tiles on +Z that we are repeating for.
				while (z2 < m_pos.z + 16 && region.getTile(x, y, z2) == tile)
					z2++;

				// The group is z2 - z tiles wide. Render it
				tileRenderer.setGroupSize(z2 - z);
				if (tileRenderer.tesselateBlockInWorldDir(pTile, x, y, z, -1.0f, -1.0f, -1.0f, direction))
				{
					// Decrease z2 as we actually want the last tile within the group.
					z2--;
					// Skip all other tiles, as they were rendered in this iteration.
					z = z2;
				}
			}
}

bool Chunk::renderLayer(TileRenderer& tileRenderer, Region& region, int layer)
{
	Tesselator& t = Tesselator::instance;
	t.begin();
	t.offset(float(-m_pos.x), float(-m_pos.y), float(-m_pos.z));

	bool bNeedAnotherLayer = false;

	int minX = m_pos.x, maxX = m_pos.x + field_10;
	int minY = m_pos.y, maxY = m_pos.y + field_14;
	int minZ = m_pos.z, maxZ = m_pos.z + field_18;

	if (ENABLE_GREEDY_MESHING && !Minecraft::useAmbientOcclusion)
	{
		renderXExpandableFaces(tileRenderer, region, layer, DIR_YNEG, bNeedAnotherLayer);
		renderXExpandableFaces(tileRenderer, region, layer, DIR_YPOS, bNeedAnotherLayer);
		renderXExpandableFaces(tileRenderer, region, layer, DIR_ZNEG, bNeedAnotherLayer);
		renderXExpandableFaces(tileRenderer, region, layer, DIR_ZPOS, bNeedAnotherLayer);
		renderZExpandableFaces(tileRenderer, region, layer, DIR_XNEG, bNeedAnotherLayer);
		renderZExpandableFaces(tileRenderer, region, layer, DIR_XPOS, bNeedAnotherLayer);
	}
	else
	{
		for (int y = minY; y < maxY; y++)
			for (int z = minZ; z < maxZ; z++)
				for (int x = minX; x < maxX; x++)
				{
					TileID tile = region.getTile(x, y, z);
					if (tile <= 0) continue;

					Tile* pTile = Tile::tiles[tile];

					if (layer == pTile->getRenderLayer())
						tileRenderer.tesselateInWorld(pTile, x, y, z);
					else
						bNeedAnotherLayer = true;
				}
	}

	if (!t.empty())
	{
		RenderChunk rchk = t.end(field_90[layer]);
		RenderChunk* pRChk = &m_renderChunks[layer];

		*pRChk = rchk;
		pRChk->field_C  = float(m_pos.x);
		pRChk->field_10 = float(m_pos.y);
		pRChk->field_14 = float(m_pos.z);

		t.offset(0.0f, 0.0f, 0.0f);

		m_bIsEmptyLayer[layer] = false;
	}
	else
	{
		t.offset(0.0f, 0.0f, 0.0f);
		t.endDrop();
	}

	return bNeedAnotherLayer;
}

void Chunk::rebuild()
{
	if (!m_bDirty)
		return;

	updates++;

	LevelChunk::touchedSky = false;

	m_bIsEmptyLayer[0] = true;
	m_bIsEmptyLayer[1] = true;

	int minX = m_pos.x, maxX = m_pos.x + field_10;
	int minY = m_pos.y, maxY = m_pos.y + field_14;
	int minZ = m_pos.z, maxZ = m_pos.z + field_18;

	Region region(m_pLevel, minX - 1, minY - 1, minZ - 1, maxX + 1, maxY + 1, maxZ + 1);
	TileRenderer tileRenderer(&region);

	for (int layer = 0; layer < 2; layer++)
	{
		bool stop = renderLayer(tileRenderer, region, layer);
		if (!stop)
			break;
	}

	field_54 = LevelChunk::touchedSky;
	field_94 = true;
}

void Chunk::translateToPos()
{
	glTranslatef(float(m_pos.x), float(m_pos.y), float(m_pos.z));
}

Chunk::Chunk(Level* level, int x, int y, int z, int a, int b, GLuint* bufs)
{
	field_4D = true;
	field_4E = false;
	field_94 = false;
	m_bDirty = false;

	m_pLevel = level;
	field_10 = a;
	field_14 = a;
	field_18 = a;
	m_pTesselator = &Tesselator::instance;
	field_8C = b;
	m_pos.x = -999;
	field_2C = Mth::sqrt(float(field_10 * field_10 + field_14 * field_14 + field_18 * field_18)) / 2;
	field_90 = bufs;

	setPos(x, y, z);
}
