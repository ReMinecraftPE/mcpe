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

class TileRenderer
{
private:
	void _init();
public:
	TileRenderer();
	TileRenderer(LevelSource*);
	float getWaterHeight(const TilePos& pos, const Material*);
	void renderTile(Tile*, TileData data, float bright = 1.0f, bool preshade = false);

	// TODO

	bool tesselateInWorld(Tile*, const TilePos& pos);
	bool tesselateInWorldNoCulling(Tile*, const TilePos& pos);
	bool tesselateInWorld(Tile*, const TilePos& pos, int textureOverride);

	void renderEast(Tile*, const Vec3& pos, int texture);
	void renderWest(Tile*, const Vec3& pos, int texture);
	void renderSouth(Tile*, const Vec3& pos, int texture);
	void renderNorth(Tile*, const Vec3& pos, int texture);
	void renderFaceDown(Tile*, const Vec3& pos, int texture);
	void renderFaceUp(Tile*, const Vec3& pos, int texture);
	void tesselateCrossTexture(Tile* tile, TileData data, const Vec3& pos);
	void tesselateTorch(Tile*, const Vec3& pos, float a, float b);
	
	bool tesselateBlockInWorldWithAmbienceOcclusion(Tile*, const TilePos& pos, float r, float g, float b);
	bool tesselateBlockInWorld(Tile*, const TilePos& pos, float r, float g, float b);
	bool tesselateBlockInWorld(Tile*, const TilePos& pos);
	bool tesselateCrossInWorld(Tile*, const TilePos& pos);
	bool tesselateWaterInWorld(Tile*, const TilePos& pos);
	bool tesselateStairsInWorld(Tile*, const TilePos& pos);
	bool tesselateFenceInWorld(Tile*, const TilePos& pos);
	bool tesselateLadderInWorld(Tile*, const TilePos& pos);
	bool tesselateTorchInWorld(Tile*, const TilePos& pos);
	bool tesselateDoorInWorld(Tile*, const TilePos& pos);
#ifndef ORIGINAL_CODE
	bool tesselateFireInWorld(Tile*, const TilePos& pos);
#endif
#ifdef ENH_USE_OWN_AO
	bool tesselateBlockInWorldWithAmbienceOcclusionV2(Tile*, const TilePos& pos, float r, float g, float b);
#endif

	int getTileColor(Tile*, const TilePos& pos);
	bool useAmbientOcclusion() const;

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
