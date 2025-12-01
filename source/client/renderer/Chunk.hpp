/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "world/tile/Tile.hpp"
#include "FrustumCuller.hpp"
#include "RenderList.hpp"
#include "Tesselator.hpp"

class Level;
class Entity;

class Chunk
{
public:
	Chunk(Level*, const TilePos& pos, int, int);

public:
	float distanceToSqr(const Entity& entity) const;
	float squishedDistanceToSqr(const Entity& entity) const;
	void reset();
	int getList(Tile::RenderLayer layer);
	RenderChunk* getRenderChunk(Tile::RenderLayer layer);
	int getAllLists(int* displayLists, int p, Tile::RenderLayer layer);
	void cull(Culler* pCuller);
	void renderBB();
	bool isEmpty();
	void setDirty();
	void setPos(const TilePos& pos);
	void setClean();
	bool isDirty();
	void rebuild();
	void translateToPos();

public:
	static int updates;

public:
	Level* m_pLevel;
	TilePos m_pos;
	TilePos m_posS;
	bool m_empty[Tile::RENDER_LAYERS_COUNT];
	TilePos m_posM;
	float field_2C;
	AABB m_aabb;
	int m_id;
	bool m_bVisible;
	bool m_bOcclusionVisible;
	bool m_bOcclusionQuerying;
	int m_occlusionId;
	bool field_54;
	RenderChunk m_renderChunks[Tile::RENDER_LAYERS_COUNT];
	Tesselator* m_pTesselator;
private:
	int m_lists;
public:
	bool m_bCompiled;
	bool m_bDirty;
};

