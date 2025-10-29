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
private:
	static bool _areCloudsAvailable;
	static bool _arePlanetsAvailable;
public:
	static bool areCloudsAvailable() { return _areCloudsAvailable; }
	static void setAreCloudsAvailable(bool value) { _areCloudsAvailable = value; }
	static bool arePlanetsAvailable() { return _arePlanetsAvailable; }
	static void setArePlanetsAvailable(bool value) { _arePlanetsAvailable = value; }

public:
	LevelRenderer(Minecraft*, Textures*);

	// LevelListener overrides
	void allChanged() override;
	void entityAdded(Entity*) override;
	void tileChanged(const TilePos& pos) override;
	void setTilesDirty(const TilePos& min, const TilePos& max) override;
	void takePicture(TripodCamera*, Entity*) override;
	void addParticle(const std::string&, const Vec3& pos, const Vec3& dir) override;
	void playSound(const std::string& name, const Vec3& pos, float volume, float pitch) override;
	void skyColorChanged() override;
	void levelEvent(Player* pPlayer, LevelEvent::ID eventId, const TilePos& pos, LevelEvent::Data data) override;

	void generateSky();
	void generateStars();
	void cull(Culler*, float);
	void deleteChunks();
	void resortChunks(const TilePos& pos);
	std::string gatherStats1();
	std::string gatherStats2();
	void onGraphicsReset();
	void render(const AABB& aabb) const;
	void render(Mob* pMob, int a, float b);
	void renderEntities(Vec3 pos, Culler*, float f);
	void renderSky(float);
	void renderClouds(float);
	void renderAdvancedClouds(float);
	void checkQueryResults(int, int);
	void renderSameAsLast(int, float);
	int  renderChunks(int start, int end, int a, float b);
	void setLevel(Level*);
	void setDirty(const TilePos& min, const TilePos& max);
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
	int m_noEntityRenderFrames;
	int m_totalEntities;
	int m_renderedEntities;
	int m_culledEntities;
	std::vector<Chunk*> field_24;
	int field_30;
	RenderList m_renderList;
	int m_totalChunks;
	int m_offscreenChunks;
	int m_occludedChunks;
	int m_renderedChunks;
	int m_emptyChunks;
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
	GLuint  m_starBuffer;
	int     m_starBufferCount;
	GLuint  m_darkBuffer;
	int     m_darkBufferCount;
	//...
	Textures* m_pTextures;
};
