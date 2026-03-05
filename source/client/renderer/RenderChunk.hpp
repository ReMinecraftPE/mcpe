/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "compat/LegacyCPP.hpp"
#include "renderer/Mesh.hpp"
#include "client/renderer/RenderChunkBuilder.hpp"
#include "client/renderer/VisibilityExtimator.hpp"
#include "world/level/TilePos.hpp"
#include "world/level/levelgen/chunk/Bounds.hpp"
#include "world/phys/AABB.hpp"

//class RenderChunkBuilder;

enum TerrainLayer
{
	TERRAIN_LAYER_OPAQUE,
	TERRAIN_LAYER_OPAQUE_SEASONS,
	TERRAIN_LAYER_DOUBLE_SIDED,
	TERRAIN_LAYER_ALPHATEST,
	TERRAIN_LAYER_ALPHATEST_SINGLE_SIDE,
	TERRAIN_LAYER_ALPHATEST_SEASONS,
	TERRAIN_LAYER_BLEND,
	TERRAIN_LAYER_FAR,
	TERRAIN_LAYER_SEASONS_FAR,
	TERRAIN_LAYER_SEASONS_FAR_ALPHATEST,
	TERRAIN_LAYERS_MIN = TERRAIN_LAYER_OPAQUE,
	TERRAIN_LAYERS_MAX = TERRAIN_LAYER_SEASONS_FAR_ALPHATEST,
	TERRAIN_LAYERS_COUNT
};

class RenderChunk
{
private:
	static bool _isUnderwater;
public:
	static mce::MaterialPtr fogMaterialMap[10];
	static mce::MaterialPtr materialMap[10];
	static mce::MaterialPtr fadingSeasonsAlphaChunkMaterial;
	static mce::MaterialPtr fadingSeasonsChunksMaterial;
	static mce::MaterialPtr fadingChunksMaterial;

public:
	enum State
	{
		BS_UNK0,
		BS_UNK1,
		BS_UNK2,
		BS_UNK3,
		BS_UNK4
	};

	enum DataState
	{
		DS_WONT_LOAD,
		DS_LOADING,
		DS_LOADED
	};

private:
	TilePos m_pos;
	TilePos m_centerPos;
	Bounds m_bounds;
	AABB m_aabb;
	float m_distanceFromPlayerSquared;
	mce::Mesh m_mesh;
	std::unique_ptr<RenderChunkBuilder> m_builder;
	bool m_unk184;
	std::atomic<State> m_state;
	bool m_dirty;
	VisibilityNode m_visibilityNode;
	bool m_unk199;
	TilePos m_neighborPositions[Facing::COUNT];
	bool m_unk272;
	bool m_unk280;
	float m_unk284;
	bool m_unk288;
	double m_unk296;

private:
	void _init(); // TODO: remove
    void _init(RenderChunk& other); // TODO: remove

public:
	RenderChunk() { _init(); } // TODO: remove
	RenderChunk(const TilePos&);
	MC_CTOR_MOVE(RenderChunk);
	RenderChunk(const TilePos& pos, mce::Mesh& mesh); // TODO: remove
	
private:
	const mce::MaterialPtr& _chooseMaterial(TerrainLayer layer, double a, bool fog);
    
public:
    void _move(RenderChunk& other); // TODO: private
	
public:
	const TilePos& getPos() const
	{
		return m_pos;
	}
	void setPos(const TilePos& pos);

	void rebuild(bool transparentLeaves);
	void startRebuild(std::unique_ptr<RenderChunkBuilder>&);
	void endRebuild();

	void reset();
	void makeReadyAsEmpty();

	TilePos getFacing(int i) const;

	DataState getDataState(TileSource& source);
	bool isRebuildState(State state) const;
	bool isPending() const;
	bool isDirty() const;
	bool setDirty(bool dirty);

	bool hasImmediateChange() const;

	float distanceToSqr(const Entity* entity);
	void updateDistanceFromPlayer(const Vec3& playerPos);

	void render(TerrainLayer layer, double a, bool fog);

protected:
	bool _tryChangeState(State, State);
	void _changeState(State, State);
	
public:
	MC_FUNC_MOVE(RenderChunk);

private:
	static void _InitLayers(mce::MaterialPtr* materials, const std::string& suffix);
public:
	static void InitMaterials();
	static void SetUnderwater(bool isUnderwater);
};

