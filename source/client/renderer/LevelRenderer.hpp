/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <algorithm>
#include <sstream>
#include "thirdparty/GL/GL.hpp"
#include "world/level/LevelListener.hpp"
#include "Textures.hpp"
#include "RenderList.hpp"
#include "TileRenderer.hpp"

class Minecraft;

class DistanceChunkSorter
{
	Mob* m_pMob;

public:
	DistanceChunkSorter(Mob* pMob)
	{
		m_pMob = pMob;
	}

	bool operator()(const Chunk* a, const Chunk* b)
	{
		float d1 = a->distanceToSqr(m_pMob);
		float d2 = b->distanceToSqr(m_pMob);

		if (d1 > 1024.0f && a->m_pos.y <= 63) d1 *= 10.0f;
		if (d2 > 1024.0f && b->m_pos.y <= 63) d2 *= 10.0f;

		return d1 < d2;
	}
};

class DirtyChunkSorter
{
	Mob* m_pMob;

public:
	DirtyChunkSorter(Mob* pMob)
	{
		m_pMob = pMob;
	}

	bool operator()(const Chunk* a, const Chunk* b)
	{
		if (a->m_bVisible && !b->m_bVisible)
			return false;
		if (!a->m_bVisible && b->m_bVisible)
			return true;

		float d1 = a->distanceToSqr(m_pMob);
		float d2 = b->distanceToSqr(m_pMob);

		if (d1 < d2) return false;
		if (d1 > d2) return true;

		return a->field_48 > b->field_48;
	}
};

class LevelRenderer : public LevelListener
{
public:
	LevelRenderer(Minecraft*, Textures*);

	void allChanged() override;
	void entityAdded(Entity*) override;
	void tileChanged(int, int, int) override;
	void setTilesDirty(int, int, int, int, int, int) override;
	void takePicture(TripodCamera*, Entity*) override;
	void addParticle(const std::string&, float, float, float, float, float, float) override;
	void playSound(const std::string& name, float x, float y, float z, float volume, float pitch) override;
	void skyColorChanged() override;
	void generateSky();
	void cull(Culler*, float);
	void deleteChunks();
	void resortChunks(int x, int y, int z);
	std::string gatherStats1();
	void onGraphicsReset();
	void render(const AABB& aabb) const;
	void render(Mob* pMob, int a, float b);
	void renderEntities(Vec3 pos, Culler*, float f);
	void renderSky(float);
	void renderClouds(float);
	void checkQueryResults(int, int);
	void renderSameAsLast(int, float);
	int  renderChunks(int start, int end, int a, float b);
	void setLevel(Level*);
	void setDirty(int, int, int, int, int, int);
	void tick();
	bool updateDirtyChunks(Mob* pMob, bool b);
	void renderHit(Player* pPlayer, const HitResult& hr, int, void*, float);
	void renderHitSelect(Player* pPlayer, const HitResult& hr, int, void*, float);
	void renderHitOutline(Player* pPlayer, const HitResult& hr, int, void*, float);

public:
	float field_4;
	float field_8;
	float field_C;
	float field_10;
	int field_14;
	int field_18;
	int field_1C;
	int field_20;
	std::vector<Chunk*> field_24;
	int field_30;
	RenderList m_renderList;
	int field_54;
	int field_58;
	int field_5C;
	int field_60;
	int field_64;
	int field_68;
	int m_resortedMinX;
	int m_resortedMinY;
	int m_resortedMinZ;
	int m_resortedMaxX;
	int m_resortedMaxY;
	int m_resortedMaxZ;
	Level* m_pLevel;
	std::vector<Chunk*> field_88;
	Chunk** m_chunks;
	Chunk** field_98;
	int m_chunksLength;
	TileRenderer* m_pTileRenderer;
	int field_A4;
	int field_A8;
	int field_AC;
	int field_B0;
	Minecraft* m_pMinecraft;
	bool field_B8;
	int field_BC;
	int m_ticksSinceStart;
	//...
	int m_nBuffers;
	GLuint* m_pBuffers;
	GLuint  m_skyBuffer;
	int     m_skyBufferCount;
	//...
	Textures* m_pTextures;
};
