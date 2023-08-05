/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "world/level/Region.hpp"
#include "client/renderer/Chunk.hpp"
#include "client/renderer/Tesselator.hpp"

#ifndef ENH_SHADE_HELD_TILES
#define RENDER_TILE_ARG_PATCH
#else
#define RENDER_TILE_ARG_PATCH , float bright
#endif

class TileRenderer
{
public:
	TileRenderer();
	TileRenderer(LevelSource*);
	float getWaterHeight(int x, int y, int z, const Material*);
	void renderTile(Tile*, int data RENDER_TILE_ARG_PATCH);

	// TODO

	bool tesselateInWorld(Tile*, int x, int y, int z);
	bool tesselateInWorldNoCulling(Tile*, int x, int y, int z);
	bool tesselateInWorld(Tile*, int x, int y, int z, int textureOverride);

	void renderEast(Tile*, float x, float y, float z, int texture);
	void renderWest(Tile*, float x, float y, float z, int texture);
	void renderSouth(Tile*, float x, float y, float z, int texture);
	void renderNorth(Tile*, float x, float y, float z, int texture);
	void renderFaceDown(Tile*, float x, float y, float z, int texture);
	void renderFaceUp(Tile*, float x, float y, float z, int texture);
	void tesselateCrossTexture(Tile* tile, int data, float x, float y, float z);
	void tesselateTorch(Tile*, float x, float y, float z, float a, float b);
	
	bool tesselateBlockInWorldWithAmbienceOcclusion(Tile*, int x, int y, int z, float r, float g, float b);
	bool tesselateBlockInWorld(Tile*, int x, int y, int z, float r, float g, float b);
	bool tesselateBlockInWorld(Tile*, int x, int y, int z);
	bool tesselateCrossInWorld(Tile*, int x, int y, int z);
	bool tesselateWaterInWorld(Tile*, int x, int y, int z);
	bool tesselateStairsInWorld(Tile*, int x, int y, int z);
	bool tesselateLadderInWorld(Tile*, int x, int y, int z);
	bool tesselateTorchInWorld(Tile*, int x, int y, int z);
	bool tesselateDoorInWorld(Tile*, int x, int y, int z);
#ifndef ORIGINAL_CODE
	bool tesselateFireInWorld(Tile*, int x, int y, int z);
#endif
#ifdef ENH_USE_OWN_AO
	bool tesselateBlockInWorldWithAmbienceOcclusionV2(Tile*, int x, int y, int z, float r, float g, float b);
#endif

	static bool canRender(int renderShape);

private:
	LevelSource* m_pLevelSource;
	int m_textureOverride = -1;
	bool field_8 = false;
	bool m_bDisableCulling = false;
	bool m_bAmbientOcclusion = false;
	float field_C = 0;
	float field_10 = 0;
	float field_14 = 0;
	float field_18 = 0;
	float field_1C = 0;
	float field_20 = 0;
	float field_24 = 0;
	float field_28 = 0;
	float field_2C = 0;
	float field_30 = 0;
	float field_34 = 0;
	float field_38 = 0;
	float field_3C = 0;
	float field_40 = 0;
	float field_44 = 0;
	float field_48 = 0;
	float field_4C = 0;
	float field_50 = 0;
	float field_54 = 0;
	float field_58 = 0;
	float field_5C = 0;
	float field_60 = 0;
	float field_64 = 0;
	float field_68 = 0;
	float field_6C = 0;
	float field_70 = 0;
	float field_74 = 0;
	//...
	int field_78 = 1;
	// for A.O.
	//red
	float m_vtxRed[4];
	//green
	float m_vtxGreen[4];
	//blue
	float m_vtxBlue[4];

	bool field_AC = false;
	bool field_AD = false;
	bool field_AE = false;
	bool field_AF = false;
	bool field_B0 = false;
	bool field_B1 = false;
	bool field_B2 = false;
	bool field_B3 = false;
	bool field_B4 = false;
	bool field_B5 = false;
	bool field_B6 = false;
	bool field_B7 = false;
};

