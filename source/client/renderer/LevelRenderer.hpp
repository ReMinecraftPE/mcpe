/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <algorithm>
#include <sstream>
#include "client/app/AppPlatformListener.hpp"
#include "renderer/hal/interface/FogState.hpp"
#include "client/renderer/FrustumCuller.hpp"
#include "world/level/LevelListener.hpp"
#include "Textures.hpp"
#include "RenderList.hpp"
#include "TileRenderer.hpp"

class Minecraft;

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
		mce::MaterialPtr sunrise;
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
	const Color& _getFogColor() const;
	void _updateViewArea(const Entity& camera);
	void _startFrame(FrustumCuller& culler, float renderDistance, float f);
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
	void levelEvent(const LevelEvent& event) override;

	void cull(Culler*, float);
	void deleteChunks();
	void resortChunks(const TilePos& pos);
	std::string gatherStats1();
	std::string gatherStats2();
	void onGraphicsReset();
	void renderLineBox(const AABB& aabb, const mce::MaterialPtr& material, float lineWidth) const;
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
	void setDimension(Dimension*);
	void setDirty(const TilePos& min, const TilePos& max);
	void tick();
	bool updateDirtyChunks(const Entity& camera, bool b);
	void renderCracks(TileSource& source, const Entity& camera, const HitResult& hr, int mode, const ItemStack* inventoryItem, float a);
	void renderHitSelect(TileSource& source, const Entity& camera, const HitResult& hr, int mode, const ItemStack* inventoryItem, float a);
	void renderHitOutline(TileSource& source, const Entity& camera, const HitResult& hr, int mode, const ItemStack* inventoryItem, float a);

protected:
	Vec3 m_viewPos;
	Materials m_materials;
	double m_initTime;
	mce::FogStateDescription m_lastFogState;
	Vec2 m_fogControl;
public:
	Vec3 m_posPrev;
	float m_destroyProgress;
	int m_noEntityRenderFrames;
	int m_totalEntities;
	int m_renderedEntities;
	int m_culledEntities;
	int m_cullStep;
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
	Dimension* m_pDimension;
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
