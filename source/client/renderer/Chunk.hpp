/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "common/Utils.hpp"
#include "FrustumCuller.hpp"
#include "RenderList.hpp"
#include "Tesselator.hpp"

class Level;
class Entity;
class Region;
class TileRenderer;

class Chunk
{
public:
	Chunk(Level*, int, int, int, int, int, GLuint*);

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
	void setPos(int x, int y, int z);
	void setClean();
	bool isDirty();
	void rebuild();
	void translateToPos();

public:
	static int updates;

public:
	Level* m_pLevel;
	Pos m_pos;
	int field_10;
	int field_14;
	int field_18;
	bool field_1C[2];
	Pos m_pos2;
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

private:
	void renderFaceDown();
	void renderFaceUp();
	void renderNorth();
	void renderSouth();
	void renderWest();
	void renderEast();

	bool renderLayer(TileRenderer& tre, Region& rgn, int minX, int minY, int minZ, int maxX, int maxY, int maxZ, int layerNo);
};

