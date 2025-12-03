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
#include "client/app/AppPlatformListener.hpp"
#include "world/level/LevelListener.hpp"
#include "Textures.hpp"
#include "RenderList.hpp"
#include "TileRenderer.hpp"

class Minecraft;

class DistanceChunkSorter
{
	const Entity& m_entity;

public:
	DistanceChunkSorter(const Entity& entity)
		: m_entity(entity)
	{
	}

	bool operator()(const Chunk* a, const Chunk* b)
	{
		float d1 = a->distanceToSqr(m_entity);
		float d2 = b->distanceToSqr(m_entity);

		if (d1 > 1024.0f && a->m_pos.y <= 63) d1 *= 10.0f;
		if (d2 > 1024.0f && b->m_pos.y <= 63) d2 *= 10.0f;

		return d1 < d2;
	}
};

class DirtyChunkSorter
{
	const Entity& m_entity;

public:
	DirtyChunkSorter(const Entity& entity)
		: m_entity(entity)
	{
	}

	bool operator()(const Chunk* a, const Chunk* b)
	{
		if (a->m_bVisible && !b->m_bVisible)
			return false;
		if (!a->m_bVisible && b->m_bVisible)
			return true;

		float d1 = a->distanceToSqr(m_entity);
		float d2 = b->distanceToSqr(m_entity);

		if (d1 < d2) return false;
		if (d1 > d2) return true;

		return a->m_id > b->m_id;
	}
};

class LevelRenderer : public LevelListener, public AppPlatformListener
{
protected:
	class Materials
	{
	public:
		mce::MaterialPtr shadow_back;
		mce::MaterialPtr shadow_front;
		mce::MaterialPtr shadow_overlay;
		mce::MaterialPtr shadow_image_overlay;
		mce::MaterialPtr stars;
		mce::MaterialPtr skyplane;
		mce::MaterialPtr sun_moon;
		mce::MaterialPtr selection_overlay;
		mce::MaterialPtr selection_overlay_opaque;
		mce::MaterialPtr selection_overlay_double_sided;
		mce::MaterialPtr selection_box;
		mce::MaterialPtr cracks_overlay;
		mce::MaterialPtr cracks_overlay_tile_entity;
		mce::MaterialPtr clouds;

		Materials();
	};

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

protected:
	void _buildSkyMesh();
	void _buildStarsMesh();
	void _buildSunAndMoonMeshes();
	void _buildShadowVolume();
	void _buildShadowOverlay();
	void _initResources();
	void _renderSunrise(float alpha);
	void _renderSolarSystem(float alpha);
	void _renderSunAndMoon(float alpha);
	void _renderStars(float alpha);
	void _renderTileShadow(Tile* tt, const Vec3& pos, TilePos& tilePos, float pow, float r, const Vec3& oPos);
	void _recreateTessellators();
	void _setupFog(const Entity& camera, int i);
	const mce::MaterialPtr& _chooseOverlayMaterial(Tile::RenderLayer layer) const;

public:
	// AppPlatformListener overrides
	void onLowMemory() override;
	void onAppResumed() override;
	void onAppSuspended() override;

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

	void cull(Culler*, float);
	void deleteChunks();
	void resortChunks(const TilePos& pos);
	std::string gatherStats1();
	std::string gatherStats2();
	void onGraphicsReset();
	void render(const AABB& aabb, const mce::MaterialPtr& material) const;
	void render(const Entity& camera, Tile::RenderLayer layer, float alpha, bool fog = false);
	void renderLevel(const Entity& camera, FrustumCuller& culler, float a1, float f);
	void renderEntities(Vec3 pos, Culler*, float f);
	void renderShadow(const Entity& entity, const Vec3& pos, float r, float pow, float a);
	void renderSky(const Entity& camera, float alpha);
	void prepareAndRenderClouds(const Entity& camera, float f);
	void renderClouds(const Entity& camera, float f);
	void renderAdvancedClouds(float);
	void checkQueryResults(int, int);
	void renderSameAsLast(TerrainLayer layer, float alpha, bool fog);
	int  renderChunks(int start, int end, Tile::RenderLayer layer, float alpha, bool fog);
	const Color& setupClearColor(float f);
	void setLevel(Level*);
	void setDirty(const TilePos& min, const TilePos& max);
	void tick();
	bool updateDirtyChunks(const Entity& camera, bool b);
	void renderCracks(const Entity& camera, const HitResult& hr, int mode, const ItemInstance* inventoryItem, float a);
	void renderHitSelect(const Entity& camera, const HitResult& hr, int mode, const ItemInstance* inventoryItem, float a);
	void renderHitOutline(const Entity& camera, const HitResult& hr, int mode, const ItemInstance* inventoryItem, float a);

protected:
	Materials m_materials;
public:
	Vec3 m_posPrev;
	float m_destroyProgress;
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
	int m_xMinChunk;
	int m_yMinChunk;
	int m_zMinChunk;
	int m_xMaxChunk;
	int m_yMaxChunk;
	int m_zMaxChunk;
	Level* m_pLevel;
	std::vector<Chunk*> m_dirtyChunks;
	Chunk** m_chunks;
	Chunk** m_sortedChunks;
	int m_chunksLength;
	TileRenderer* m_pTileRenderer;
	int m_xChunks;
	int m_yChunks;
	int m_zChunks;
	int m_chunkLists;
	Minecraft* m_pMinecraft;
	bool m_bOcclusionCheck;
	int m_lastViewDistance;
	int m_ticksSinceStart;
	Color m_fogColor;
	float m_fogBrO;
	float m_fogBr;
	//...
	//mce::Mesh m_shadowVolumeMesh;
	//mce::Mesh m_shadowOverlayMesh;
	mce::Mesh m_skyMesh;
	mce::Mesh m_cloudsMesh;
	mce::Mesh m_starsMesh;
	mce::Mesh m_darkMesh;
	//...
	Textures* m_pTextures;
};
