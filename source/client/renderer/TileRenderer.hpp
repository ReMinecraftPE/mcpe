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
private:
	void _init();
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
	bool tesselateFireInWorld(Tile*, int x, int y, int z);
	bool tesselateWireInWorld(Tile*, int x, int y, int z);
	bool tesselateLeverInWorld(Tile*, int x, int y, int z);
	bool tesselateDiodeInWorld(Tile*, int x, int y, int z);

#ifdef ENH_USE_OWN_AO
	bool tesselateBlockInWorldWithAmbienceOcclusionV2(Tile*, int x, int y, int z, float r, float g, float b);
#endif

	int getTileColor(Tile*, int x, int y, int z);

	static bool canRender(int renderShape);

	static bool m_bFancyGrass;
	static bool m_bBiomeColors;

private:
	LevelSource* m_pLevelSource;
	int m_textureOverride;
	bool field_8;
	bool m_bDisableCulling;
	bool m_bAmbientOcclusion;
	float field_C;
	float field_10;
	float field_14;
	float field_18;
	float field_1C;
	float field_20;
	float field_24;
	float field_28;
	float field_2C;
	float field_30;
	float field_34;
	float field_38;
	float field_3C;
	float field_40;
	float field_44;
	float field_48;
	float field_4C;
	float field_50;
	float field_54;
	float field_58;
	float field_5C;
	float field_60;
	float field_64;
	float field_68;
	float field_6C;
	float field_70;
	float field_74;
	//...
	int field_78;
	// for A.O.
	//red
	float m_vtxRed[4];
	//green
	float m_vtxGreen[4];
	//blue
	float m_vtxBlue[4];

	bool field_AC;
	bool field_AD;
	bool field_AE;
	bool field_AF;
	bool field_B0;
	bool field_B1;
	bool field_B2;
	bool field_B3;
	bool field_B4;
	bool field_B5;
	bool field_B6;
	bool field_B7;
};

