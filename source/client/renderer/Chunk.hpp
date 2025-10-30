/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "FrustumCuller.hpp"
#include "RenderList.hpp"
#include "Tesselator.hpp"

class Level;
class Entity;

class Chunk
{
public:
	Chunk(Level*, const TilePos& pos, int, int, GLuint*);

	float distanceToSqr(const Entity*) const;
	float squishedDistanceToSqr(const Entity*) const;
	void reset();
	int getList(int idx);
	RenderChunk* getRenderChunk(int idx);
	int getAllLists(int* arr, int arr_idx, int idx);
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
	TilePos field_10;
	bool field_1C[2];
	TilePos m_pos2;
	float field_2C;
	AABB m_aabb;
	int field_48;
	bool m_bVisible;
	bool field_4D;
	bool field_4E;
	int field_50;
	bool field_54;
	RenderChunk m_renderChunks[2];
	Tesselator* m_pTesselator;
	int field_8C;
	GLuint* field_90;
	bool field_94;
	bool m_bDirty;
};

