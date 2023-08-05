/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <algorithm>
#include <sstream>

#include "compat/GL.hpp"
#include "LevelListener.hpp"
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
	void playSound(const std::string& a, float b, float c, float d, float e, float f) override;
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
	void checkQueryResults(int, int);
	void renderSameAsLast(int, float);
	int  renderChunks(int start, int end, int a, float b);
	void setLevel(Level*);
	void setDirty(int, int, int, int, int, int);
	void tick();
	bool updateDirtyChunks(Mob* pMob, bool b);
	void renderHitSelect(Player* pPlayer, const HitResult& hr, int, void*, float);

public:
	float field_4 = -9999.0f;
	float field_8 = -9999.0f;
	float field_C = -9999.0f;
	float field_10 = 0.0f;
	int field_14 = 2;
	int field_18 = 0;
	int field_1C = 0;
	int field_20 = 0;
	std::vector<Chunk*> field_24;
	int field_30 = 0;
	RenderList m_renderList;
	int field_54 = 0;
	int field_58 = 0;
	int field_5C = 0;
	int field_60 = 0;
	int field_64 = 0;
	int field_68 = 0;
	int field_6C = 0;
	int field_70 = 0;
	int field_74 = 0;
	int field_78 = 0;
	int field_7C = 0;
	int field_80 = 0;
	Level* m_pLevel = nullptr;
	std::vector<Chunk*> field_88;
	Chunk** m_chunks = nullptr;
	Chunk** field_98 = nullptr;
	int m_chunksLength = 0;
	TileRenderer* m_pTileRenderer = nullptr;
	int field_A4 = 0;
	int field_A8 = 0;
	int field_AC = 0;
	int field_B0 = 0;
	Minecraft* m_pMinecraft;
	bool field_B8 = false;
	int field_BC = -1;
	int field_C0 = 0;
	//...
	int m_nBuffers = 26136;
	GLuint* m_pBuffers;
	GLuint  field_D8;
	int     field_DC;
	//...
	Textures* m_pTextures;
};
