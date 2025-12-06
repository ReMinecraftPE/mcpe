/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "compat/LegacyCPP.hpp"
#include "renderer/Mesh.hpp"
#include "world/level/TilePos.hpp"

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

private:
	double m_lastRebuilt;
public:
	TilePos m_pos;
	mce::Mesh m_mesh;

private:
	void _init();
    void _init(RenderChunk& other);

public:
	RenderChunk() { _init(); }
	MC_CTOR_MOVE(RenderChunk);
	RenderChunk(const TilePos& pos, mce::Mesh& mesh);
	
private:
	const mce::MaterialPtr& _chooseMaterial(TerrainLayer layer, double a, bool fog);
    
public:
    void _move(RenderChunk& other);
	
public:
	void render(TerrainLayer layer, double a, bool fog);
	void reset();
	
public:
	MC_FUNC_MOVE(RenderChunk);

private:
	static void _InitLayers(mce::MaterialPtr* materials, const std::string& suffix);
public:
	static void InitMaterials();
	static void SetUnderwater(bool isUnderwater);
};

