/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TileRenderer.hpp"
#include "client/app/Minecraft.hpp"
#include "client/renderer/PatchManager.hpp"
#include "client/renderer/GrassColor.hpp"
#include "client/renderer/FoliageColor.hpp"
#include "client/renderer/renderer/RenderMaterialGroup.hpp"
#include "world/tile/FireTile.hpp"
#include "world/tile/RedStoneDustTile.hpp"
#include "world/tile/GrassTile.hpp"
#include "world/tile/LiquidTile.hpp"
#include "world/tile/DiodeTile.hpp"
#include "world/tile/LeafTile.hpp"

#include "world/tile/FenceTile.hpp"
#include "world/level/TileSource.hpp"
#include "GameMods.hpp"

#define DEFAULT_LIGHT_COLOR 0xFF00FF

static const float _shouldPreshadeFace[Facing::COUNT] = {
	/* DOWN  */ true,
	/* UP    */ false,
	/* NORTH */ true,
	/* SOUTH */ true,
	/* WEST  */ true,
	/* EAST  */ true
};

static const Vec3* _faceNormals[Facing::COUNT] = {
	/* DOWN  */ &Vec3::NEG_UNIT_Y,
	/* UP    */ &Vec3::UNIT_Y,
	/* NORTH */ &Vec3::NEG_UNIT_Z,
	/* SOUTH */ &Vec3::UNIT_Z,
	/* WEST  */ &Vec3::NEG_UNIT_X,
	/* EAST  */ &Vec3::UNIT_X
};

static const float _faceShades[TileRenderer::LIGHTING_PRESETS_COUNT][Facing::COUNT] = {
	/* JAVA */
	{
		/* DOWN  */ 0.5f,
		/* UP    */ 0.0f,
		/* NORTH */ 0.63f,
		/* SOUTH */ 0.63f,
		/* WEST  */ 0.43f,
		/* EAST  */ 0.43f
	},
	/* CONSOLE */
	{
		/* DOWN  */ 0.5f,
		/* UP    */ 0.0f,
		/* NORTH */ 0.4f,
		/* SOUTH */ 0.4f,
		/* WEST  */ 0.74f,
		/* EAST  */ 0.74f
	}
};

TileRenderer::Materials::Materials()
{
	MATERIAL_PTR(common, ui_item);
}

void TileRenderer::_init()
{
	m_fixedTexture = -1;
	m_bXFlipTexture = false;
	m_bNoCulling = false;
	m_bRenderingGui = false;
	m_ambientOcclusion = false;
	m_lightingPreset = LIGHTING_PRESET_JAVA;

	// AO stuff
	field_C = 0;
	field_10 = 0;
	field_14 = 0;
	field_18 = 0;
	field_1C = 0;
	field_20 = 0;
	field_24 = 0;
	field_28 = 0;
	field_2C = 0;
	field_30 = 0;
	field_34 = 0;
	field_38 = 0;
	field_3C = 0;
	field_40 = 0;
	field_44 = 0;
	field_48 = 0;
	field_4C = 0;
	field_50 = 0;
	field_54 = 0;
	field_58 = 0;
	field_5C = 0;
	field_60 = 0;
	field_64 = 0;
	field_68 = 0;
	field_6C = 0;
	field_70 = 0;
	field_74 = 0;
	field_78 = 1;
	field_AC = false;
	field_AD = false;
	field_AE = false;
	field_AF = false;
	field_B0 = false;
	field_B1 = false;
	field_B2 = false;
	field_B3 = false;
	field_B4 = false;
	field_B5 = false;
	field_B6 = false;
	field_B7 = false;
}

TileRenderer::TileRenderer(Tesselator& tessellator, TileSource* tileSource)
	: m_tessellator(tessellator)
{
	_init();
	m_pTileSource = tileSource;
}

// tex1 should be mandatory to keep opengl happy as there are no explicit padding available in glsl
//   and also for the benefits of using vanilla shaders / modded shaders back in the day
//
// in terms of the content of tex1, it is a lookup table of light colors since JE b1.8/ PE 0.10
//   this as a mod in the future, or tex1 influence can be oblterated from the shader for now
//   so it falls back to the monochromatic lighting seen in old versions
void TileRenderer::_tex1(const Vec2& uv)
{
	
	//if (m_bRenderingGui)
	//	return;

	//constexpr float s = 0.0039062f;

	m_tessellator.tex1(uv);
}

Vec2 TileRenderer::getLightColor(const Tile* tile, const TilePos& pos)
{
	if (m_bRenderingGui)
		return Vec2::ONE;

	if (pos.y > 127)
		return Vec2(240.0f / 255.0f, 0.0f);

	float brightness = tile->getBrightness(*m_pTileSource, pos);
	return Vec2(brightness, brightness);
}

Color TileRenderer::_getTileColor(const TilePos& pos, Tile* tile)
{
	Color color = tile->getColor(*m_pTileSource, pos);

	if (tile->isSeasonTinted())
		color.b = 1.0f;

	return color;
}

float TileRenderer::getWaterHeight(const TilePos& pos, const Material* pCheckMtl)
{
	int iBias = 0;
	float fHeight = 0.0f;
	for (int i = 0; i < 4; i++)
	{
		TilePos checkPos(pos.x - (i & 1),
					  pos.y,
					  pos.z - ((i >> 1) & 1));

		if (m_pTileSource->getMaterial(TilePos(checkPos.x, checkPos.y + 1, checkPos.z)) == pCheckMtl)
			return 1.0f;

		Material* pMtl = m_pTileSource->getMaterial(checkPos);
		if (pMtl == pCheckMtl)
		{
			TileData data = m_pTileSource->getData(checkPos);
			if (data >= 8 || data == 0)
			{
				fHeight += LiquidTile::getWaterVolume(data) * 10.0f;
				iBias += 10;
			}

			fHeight += LiquidTile::getWaterVolume(data);
			iBias++;
			continue;
		}

		if (!pMtl->isSolid())
		{
			fHeight++;
			iBias++;
		}
	}

	return 1.0f - fHeight / float(iBias);
}

bool TileRenderer::canRender(int renderShape)
{
	return renderShape == SHAPE_SOLID || renderShape == SHAPE_STAIRS || renderShape == SHAPE_FENCE || renderShape == SHAPE_FENCE_GATE || renderShape == SHAPE_CACTUS;
}

// @NOTE: This sucks! Very badly! But it's how they did it.
void TileRenderer::renderEast(Tile* tile, const Vec3& pos, int texture)
{
	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->setShape(0.0625, 0, 0, 0.9375, 1, 1);

	static constexpr float C_RATIO = 1.0f / 256.0f;

	if (m_fixedTexture >= 0)
		texture = m_fixedTexture;

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	float texU_l, texU_r, texV_u, texV_d;

	AABB& aabb = tile->m_aabb;

	if (aabb.min.z < 0.0f || aabb.max.z > 1.0f)
	{
		texU_l = C_RATIO * (texX);
		texU_r = C_RATIO * (texX + 15.99f);
	}
	// if flipping on the Z coordinate
	else if (m_bXFlipTexture)
	{
		texU_r = C_RATIO * (texX + aabb.min.z * 16);
		texU_l = C_RATIO * (texX + aabb.max.z * 16 - 0.01f);
	}
	else
	{
		texU_l = C_RATIO * (texX + aabb.min.z * 16);
		texU_r = C_RATIO * (texX + aabb.max.z * 16 - 0.01f);
	}

	if (aabb.min.y < 0.0f || aabb.max.y > 1.0f)
	{
		texV_u = C_RATIO * (texY);
		texV_d = C_RATIO * (texY + 15.99f);
	}
	else
	{
#ifdef ENH_BETA_1_7_TERRAIN_ATLAS
		texV_u = C_RATIO * (texY + (1.0f - aabb.max.y) * 16.0f);
		texV_d = C_RATIO * (texY + (1.0f - aabb.min.y) * 16.0f - 0.01f);
#else
		texV_u = C_RATIO * (texY + aabb.min.y * 16.0f);
		texV_d = C_RATIO * (texY + aabb.max.y * 16.0f - 0.01f);
#endif
	}

	Tesselator& t = m_tessellator;

	if (m_ambientOcclusion)
	{
		t.color(m_vtxRed[0], m_vtxGreen[0], m_vtxBlue[0]);
		//_tex1(m_vtxLightTex[0]);
		t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_l, texV_d);

		t.color(m_vtxRed[1], m_vtxGreen[1], m_vtxBlue[1]);
		//_tex1(m_vtxLightTex[1]);
		t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_r, texV_d);

		t.color(m_vtxRed[2], m_vtxGreen[2], m_vtxBlue[2]);
		//_tex1(m_vtxLightTex[2]);
		t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_r, texV_u);

		t.color(m_vtxRed[3], m_vtxGreen[3], m_vtxBlue[3]);
		//_tex1(m_vtxLightTex[3]);
		t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_l, texV_u);

		return;
	}
	
	t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_l, texV_d);
	t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_r, texV_d);
	t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_r, texV_u);
	t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_l, texV_u);

	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->updateShape(*m_pTileSource, pos);
}

void TileRenderer::renderWest(Tile* tile, const Vec3& pos, int texture)
{
	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->setShape(0.0625, 0, 0, 0.9375, 1, 1);

	static constexpr float C_RATIO = 1.0f / 256.0f;

	if (m_fixedTexture >= 0)
		texture = m_fixedTexture;

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	float texU_l, texU_r, texV_u, texV_d;

	AABB& aabb = tile->m_aabb;

	if (aabb.min.z < 0.0f || aabb.max.z > 1.0f)
	{
		texU_l = C_RATIO * (texX);
		texU_r = C_RATIO * (texX + 15.99f);
	}
	// if flipping on the Z coordinate
	else if (m_bXFlipTexture)
	{
		texU_r = C_RATIO * (texX + aabb.min.z * 16);
		texU_l = C_RATIO * (texX + aabb.max.z * 16 - 0.01f);
	}
	else
	{
		texU_l = C_RATIO * (texX + aabb.min.z * 16);
		texU_r = C_RATIO * (texX + aabb.max.z * 16 - 0.01f);
	}

	if (aabb.min.y < 0.0f || aabb.max.y>1.0f)
	{
		texV_u = C_RATIO * (texY);
		texV_d = C_RATIO * (texY + 15.99f);
	}
	else
	{
#ifdef ENH_BETA_1_7_TERRAIN_ATLAS
		texV_u = C_RATIO * (texY + (1.0f - aabb.max.y) * 16.0f);
		texV_d = C_RATIO * (texY + (1.0f - aabb.min.y) * 16.0f - 0.01f);
#else
		texV_u = C_RATIO * (texY + aabb.min.y * 16.0f);
		texV_d = C_RATIO * (texY + aabb.max.y * 16.0f - 0.01f);
#endif
	}

	Tesselator& t = m_tessellator;

	if (m_ambientOcclusion)
	{
		t.color(m_vtxRed[0], m_vtxGreen[0], m_vtxBlue[0]);
		//_tex1(m_vtxLightTex[0]);
		t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_r, texV_u);

		t.color(m_vtxRed[1], m_vtxGreen[1], m_vtxBlue[1]);
		//_tex1(m_vtxLightTex[1]);
		t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_l, texV_u);

		t.color(m_vtxRed[2], m_vtxGreen[2], m_vtxBlue[2]);
		//_tex1(m_vtxLightTex[2]);
		t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_l, texV_d);

		t.color(m_vtxRed[3], m_vtxGreen[3], m_vtxBlue[3]);
		//_tex1(m_vtxLightTex[3]);
		t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_r, texV_d);

		return;
	}
	
	t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_r, texV_u);
	t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_l, texV_u);
	t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_l, texV_d);
	t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_r, texV_d);

	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->updateShape(*m_pTileSource, pos);
}

void TileRenderer::renderSouth(Tile* tile, const Vec3& pos, int texture)
{
	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->setShape(0, 0, 0.0625, 1, 1, 0.9375);

	static constexpr float C_RATIO = 1.0f / 256.0f;

	if (m_fixedTexture >= 0)
		texture = m_fixedTexture;

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	float texU_l, texU_r, texV_u, texV_d;

	AABB& aabb = tile->m_aabb;

	if (aabb.min.x < 0.0f || aabb.max.x > 1.0f)
	{
		texU_l = C_RATIO * (texX);
		texU_r = C_RATIO * (texX + 15.99f);
	}
	// if flipping on the X coordinate
	else if (m_bXFlipTexture)
	{
		texU_r = C_RATIO * (texX + aabb.min.x * 16);
		texU_l = C_RATIO * (texX + aabb.max.x * 16 - 0.01f);
	}
	else
	{
		texU_l = C_RATIO * (texX + aabb.min.x * 16);
		texU_r = C_RATIO * (texX + aabb.max.x * 16 - 0.01f);
	}

	if (aabb.min.y < 0.0f || aabb.max.y>1.0f)
	{
		texV_u = C_RATIO * (texY);
		texV_d = C_RATIO * (texY + 15.99f);
	}
	else
	{
#ifdef ENH_BETA_1_7_TERRAIN_ATLAS
		texV_u = C_RATIO * (texY + (1.0f - aabb.max.y) * 16.0f);
		texV_d = C_RATIO * (texY + (1.0f - aabb.min.y) * 16.0f - 0.01f);
#else
		texV_u = C_RATIO * (texY + aabb.min.y * 16.0f);
		texV_d = C_RATIO * (texY + aabb.max.y * 16.0f - 0.01f);
#endif
	}

	Tesselator& t = m_tessellator;

	if (m_ambientOcclusion)
	{
		t.color(m_vtxRed[0], m_vtxGreen[0], m_vtxBlue[0]);
		//_tex1(m_vtxLightTex[0]);
		t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_l, texV_u);

		t.color(m_vtxRed[1], m_vtxGreen[1], m_vtxBlue[1]);
		//_tex1(m_vtxLightTex[1]);
		t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_l, texV_d);

		t.color(m_vtxRed[2], m_vtxGreen[2], m_vtxBlue[2]);
		//_tex1(m_vtxLightTex[2]);
		t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_r, texV_d);

		t.color(m_vtxRed[3], m_vtxGreen[3], m_vtxBlue[3]);
		//_tex1(m_vtxLightTex[3]);
		t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_r, texV_u);

		return;
	}

	t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_l, texV_u);
	t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_l, texV_d);
	t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_r, texV_d);
	t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_r, texV_u);

	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->updateShape(*m_pTileSource, pos);
}

void TileRenderer::renderNorth(Tile* tile, const Vec3& pos, int texture)
{
	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->setShape(0, 0, 0.0625, 1, 1, 0.9375);

	static constexpr float C_RATIO = 1.0f / 256.0f;

	if (m_fixedTexture >= 0)
		texture = m_fixedTexture;

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	float texU_l, texU_r, texV_u, texV_d;

	AABB& aabb = tile->m_aabb;

	if (aabb.min.x < 0.0f || aabb.max.x > 1.0f)
	{
		texU_l = C_RATIO * (texX);
		texU_r = C_RATIO * (texX + 15.99f);
	}
	// if flipping on the X coordinate
	else if (m_bXFlipTexture)
	{
		texU_r = C_RATIO * (texX + aabb.min.x * 16);
		texU_l = C_RATIO * (texX + aabb.max.x * 16 - 0.01f);
	}
	else
	{
		texU_l = C_RATIO * (texX + aabb.min.x * 16);
		texU_r = C_RATIO * (texX + aabb.max.x * 16 - 0.01f);
	}

	if (aabb.min.y < 0.0f || aabb.max.y>1.0f)
	{
		texV_u = C_RATIO * (texY);
		texV_d = C_RATIO * (texY + 15.99f);
	}
	else
	{
#ifdef ENH_BETA_1_7_TERRAIN_ATLAS
		texV_u = C_RATIO * (texY + (1.0f - aabb.max.y) * 16.0f);
		texV_d = C_RATIO * (texY + (1.0f - aabb.min.y) * 16.0f - 0.01f);
#else
		texV_u = C_RATIO * (texY + aabb.min.y * 16.0f);
		texV_d = C_RATIO * (texY + aabb.max.y * 16.0f - 0.01f);
#endif
	}

	Tesselator& t = m_tessellator;

	if (m_ambientOcclusion)
	{
		t.color(m_vtxRed[0], m_vtxGreen[0], m_vtxBlue[0]);
		//_tex1(m_vtxLightTex[0]);
		t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_r, texV_u);

		t.color(m_vtxRed[1], m_vtxGreen[1], m_vtxBlue[1]);
		//_tex1(m_vtxLightTex[1]);
		t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_l, texV_u);

		t.color(m_vtxRed[2], m_vtxGreen[2], m_vtxBlue[2]);
		//_tex1(m_vtxLightTex[2]);
		t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_l, texV_d);

		t.color(m_vtxRed[3], m_vtxGreen[3], m_vtxBlue[3]);
		//_tex1(m_vtxLightTex[3]);
		t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_r, texV_d);

		return;
	}

	t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_r, texV_u);
	t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_l, texV_u);
	t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_l, texV_d);
	t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_r, texV_d);

	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->updateShape(*m_pTileSource, pos);
}

void TileRenderer::renderFaceUp(Tile* tile, const Vec3& pos, int texture)
{
	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->setShape(0, 0, 0, 1, 1, 1);

	static constexpr float C_RATIO = 1.0f / 256.0f;

	if (m_fixedTexture >= 0)
		texture = m_fixedTexture;

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	float texU_1, texU_2, texV_1, texV_2;

	AABB& aabb = tile->m_aabb;

	if (aabb.min.x >= 0.0f && aabb.max.x <= 1.0f)
	{
		texU_1 = C_RATIO * (texX + 16.0f * aabb.min.x);
		texU_2 = C_RATIO * (texX + 16.0f * aabb.max.x - 0.01f);
	}
	else
	{
		texU_1 = C_RATIO * (texX);
		texU_2 = C_RATIO * (texX + 15.99f);
	}

	if (aabb.min.z >= 0.0f && aabb.max.z <= 1.0f)
	{
		texV_1 = C_RATIO * (texY + 16.0f * aabb.min.z);
		texV_2 = C_RATIO * (texY + 16.0f * aabb.max.z - 0.01f);
	}
	else
	{
		texV_1 = C_RATIO * (texY);
		texV_2 = C_RATIO * (texY + 15.99f);
	}

	Tesselator& t = m_tessellator;

	if (m_ambientOcclusion)
	{
		t.color(m_vtxRed[0], m_vtxGreen[0], m_vtxBlue[0]);
		//_tex1(m_vtxLightTex[0]);
		t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_2, texV_2);

		t.color(m_vtxRed[1], m_vtxGreen[1], m_vtxBlue[1]);
		//_tex1(m_vtxLightTex[1]);
		t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_2, texV_1);

		t.color(m_vtxRed[2], m_vtxGreen[2], m_vtxBlue[2]);
		//_tex1(m_vtxLightTex[2]);
		t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_1, texV_1);

		t.color(m_vtxRed[3], m_vtxGreen[3], m_vtxBlue[3]);
		//_tex1(m_vtxLightTex[3]);
		t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_1, texV_2);

		return;
	}

	t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_2, texV_2);
	t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_2, texV_1);
	t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_1, texV_1);
	t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_1, texV_2);

	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->updateShape(*m_pTileSource, pos);
}

void TileRenderer::renderFaceDown(Tile* tile, const Vec3& pos, int texture)
{
	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->setShape(0, 0, 0, 1, 1, 1);
	
	static constexpr float C_RATIO = 1.0f / 256.0f;

	if (m_fixedTexture >= 0)
		texture = m_fixedTexture;

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	float texU_1, texU_2, texV_1, texV_2;

	AABB& aabb = tile->m_aabb;

	if (aabb.min.x >= 0.0f && aabb.max.x <= 1.0f)
	{
		texU_1 = C_RATIO * (texX + 16.0f * aabb.min.x);
		texU_2 = C_RATIO * (texX + 16.0f * aabb.max.x - 0.01f);
	}
	else
	{
		texU_1 = C_RATIO * (texX);
		texU_2 = C_RATIO * (texX + 15.99f);
	}

	if (aabb.min.z >= 0.0f && aabb.max.z <= 1.0f)
	{
		texV_1 = C_RATIO * (texY + 16.0f * aabb.min.z);
		texV_2 = C_RATIO * (texY + 16.0f * aabb.max.z - 0.01f);
	}
	else
	{
		texV_1 = C_RATIO * (texY);
		texV_2 = C_RATIO * (texY + 15.99f);
	}

	Tesselator& t = m_tessellator;

	if (m_ambientOcclusion)
	{
		t.color(m_vtxRed[0], m_vtxGreen[0], m_vtxBlue[0]);
		//_tex1(m_vtxLightTex[0]);
		t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_1, texV_2);

		t.color(m_vtxRed[1], m_vtxGreen[1], m_vtxBlue[1]);
		//_tex1(m_vtxLightTex[1]);
		t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_1, texV_1);

		t.color(m_vtxRed[2], m_vtxGreen[2], m_vtxBlue[2]);
		//_tex1(m_vtxLightTex[2]);
		t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_2, texV_1);

		t.color(m_vtxRed[3], m_vtxGreen[3], m_vtxBlue[3]);
		//_tex1(m_vtxLightTex[3]);
		t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_2, texV_2);

		return;
	}

	t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_1, texV_2);
	t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_1, texV_1);
	t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_2, texV_1);
	t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_2, texV_2);

	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->updateShape(*m_pTileSource, pos);
}

void TileRenderer::tesselateCrossTexture(const FullTile& tile, const Vec3& pos, bool simple)
{
	static constexpr float C_RATIO = 1.0f / 256.0f;

	const Tile& tileType = *tile.getType();

	int texture = m_fixedTexture;
	if (texture < 0)
		texture = tileType.getTexture(Facing::DOWN, tile.data);

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	// calculate U and V coordinates
	float texU_l = texX * C_RATIO, texU_r = (texX + 15.99f) * C_RATIO;
	float texV_u = texY * C_RATIO, texV_d = (texY + 15.99f) * C_RATIO;

	float cenX = pos.x + 0.5f, cenZ = pos.z + 0.5f;
	float newY = pos.y;

	if (tileType.getRenderShape() == SHAPE_RANDOM_CROSS)
	{
		int64_t var17 = int64_t(pos.x * 3129871) ^ (int64_t)pos.z * 116129781L ^ (int64_t)pos.y;
		var17 = var17 * var17 * 42317861L + var17 * 11L;
		cenX += (((var17 >> 16 & 15L) / 15.0f) - 0.5f) * 0.5f;
		newY += (((var17 >> 20 & 15L) / 15.0f) - 1.0f) * 0.2f;
		cenZ += (((var17 >> 24 & 15L) / 15.0f) - 0.5f) * 0.5f;
	}
	
	float x1 = cenX - 0.45f, x2 = cenX + 0.45f;
	float z1 = cenZ - 0.45f, z2 = cenZ + 0.45f;

	Tesselator& t = m_tessellator;
	// face 1
	t.vertexUV(x1, newY + 1, z1, texU_l, texV_u);
	t.vertexUV(x1, newY + 0, z1, texU_l, texV_d);
	t.vertexUV(x2, newY + 0, z2, texU_r, texV_d);
	t.vertexUV(x2, newY + 1, z2, texU_r, texV_u);

	// Check if we're culling the other faces
	if (!simple)
	{
		// face 2
		t.vertexUV(x2, newY + 1, z2, texU_l, texV_u);
		t.vertexUV(x2, newY + 0, z2, texU_l, texV_d);
		t.vertexUV(x1, newY + 0, z1, texU_r, texV_d);
		t.vertexUV(x1, newY + 1, z1, texU_r, texV_u);
	}

	// face 3
	t.vertexUV(x1, newY + 1, z2, texU_l, texV_u);
	t.vertexUV(x1, newY + 0, z2, texU_l, texV_d);
	t.vertexUV(x2, newY + 0, z1, texU_r, texV_d);
	t.vertexUV(x2, newY + 1, z1, texU_r, texV_u);

	if (!simple)
	{
		// face 4
		t.vertexUV(x2, newY + 1, z1, texU_l, texV_u);
		t.vertexUV(x2, newY + 0, z1, texU_l, texV_d);
		t.vertexUV(x1, newY + 0, z2, texU_r, texV_d);
		t.vertexUV(x1, newY + 1, z2, texU_r, texV_u);
	}
}

void TileRenderer::tesselateRowTexture(Tile* tile, int data, const Vec3& pos)
{
	static constexpr float C_RATIO = 1.0f / 256.0f;

	int texture = m_fixedTexture;
	if (texture < 0)
		texture = tile->getTexture(Facing::DOWN, data);

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	// calculate U and V coordinates
	float u0 = texX * C_RATIO, u1 = (texX + 15.99f) * C_RATIO;
	float v0 = texY * C_RATIO, v1 = (texY + 15.99f) * C_RATIO;


	float x0 = pos.x + 0.25f, x1 = pos.x + 0.75f;
	float z0 = pos.z, z1 = pos.z + 1.0f;

	Tesselator& t = m_tessellator;
	t.vertexUV(x0, pos.y + 1.0, z0, u0, v0);
	t.vertexUV(x0, pos.y + 0.0, z0, u0, v1);
	t.vertexUV(x0, pos.y + 0.0, z1, u1, v1);
	t.vertexUV(x0, pos.y + 1.0, z1, u1, v0);
	t.vertexUV(x0, pos.y + 1.0, z1, u0, v0);
	t.vertexUV(x0, pos.y + 0.0, z1, u0, v1);
	t.vertexUV(x0, pos.y + 0.0, z0, u1, v1);
	t.vertexUV(x0, pos.y + 1.0, z0, u1, v0);
	t.vertexUV(x1, pos.y + 1.0, z1, u0, v0);
	t.vertexUV(x1, pos.y + 0.0, z1, u0, v1);
	t.vertexUV(x1, pos.y + 0.0, z0, u1, v1);
	t.vertexUV(x1, pos.y + 1.0, z0, u1, v0);
	t.vertexUV(x1, pos.y + 1.0, z0, u0, v0);
	t.vertexUV(x1, pos.y + 0.0, z0, u0, v1);
	t.vertexUV(x1, pos.y + 0.0, z1, u1, v1);
	t.vertexUV(x1, pos.y + 1.0, z1, u1, v0);
	x0 = pos.x + 0.5 - 0.5;
	x1 = pos.x + 0.5 + 0.5;
	z0 = pos.z + 0.5 - 0.25;
	z1 = pos.z + 0.5 + 0.25;
	t.vertexUV(x0, pos.y + 1.0, z0, u0, v0);
	t.vertexUV(x0, pos.y + 0.0, z0, u0, v1);
	t.vertexUV(x1, pos.y + 0.0, z0, u1, v1);
	t.vertexUV(x1, pos.y + 1.0, z0, u1, v0);
	t.vertexUV(x1, pos.y + 1.0, z0, u0, v0);
	t.vertexUV(x1, pos.y + 0.0, z0, u0, v1);
	t.vertexUV(x0, pos.y + 0.0, z0, u1, v1);
	t.vertexUV(x0, pos.y + 1.0, z0, u1, v0);
	t.vertexUV(x1, pos.y + 1.0, z1, u0, v0);
	t.vertexUV(x1, pos.y + 0.0, z1, u0, v1);
	t.vertexUV(x0, pos.y + 0.0, z1, u1, v1);
	t.vertexUV(x0, pos.y + 1.0, z1, u1, v0);
	t.vertexUV(x0, pos.y + 1.0, z1, u0, v0);
	t.vertexUV(x0, pos.y + 0.0, z1, u0, v1);
	t.vertexUV(x1, pos.y + 0.0, z1, u1, v1);
	t.vertexUV(x1, pos.y + 1.0, z1, u1, v0);
}

bool TileRenderer::tesselateBlockInWorld(Tile* tile, const TilePos& pos, float r, float g, float b)
{
	float topR = r, topG = g, topB = b;
	bool hasFancyGrass = false;

	if (tile == Tile::grass)
	{
		r = g = b = 1.0f;
		hasFancyGrass = ((GrassTile*)tile)->m_bFancyGrass;
	}

	Tesselator& t = m_tessellator;

	float fLightHere = tile->getBrightness(*m_pTileSource, pos);
	bool bDrewAnything = false;

	if (m_bNoCulling || tile->shouldRenderFace(*m_pTileSource, pos.below(), Facing::DOWN))
	{
		bDrewAnything = true;

		float fLight = tile->getBrightness(*m_pTileSource, pos.below());
		t.color(r * 0.5f * fLight, g * 0.5f * fLight, b * 0.5f * fLight);

		renderFaceDown(tile, pos, tile->getTexture(*m_pTileSource, pos, Facing::DOWN));
	}

	if (m_bNoCulling || tile->shouldRenderFace(*m_pTileSource, pos.above(), Facing::UP))
	{
		bDrewAnything = true;

		float fLight = tile->getBrightness(*m_pTileSource, pos.above());
		if (tile->m_aabb.max.y != 1.0f && !tile->m_pMaterial->isLiquid())
			fLight = fLightHere;

		t.color(topR * fLight, topG * fLight, topB * fLight);

		renderFaceUp(tile, pos, tile->getTexture(*m_pTileSource, pos, Facing::UP));
	}

	if (m_bNoCulling || tile->shouldRenderFace(*m_pTileSource, pos.north(), Facing::NORTH))
	{
		bDrewAnything = true;

		float fLight = tile->getBrightness(*m_pTileSource, pos.north());
		if (tile->m_aabb.min.z > 0.0f)
			fLight = fLightHere;

		t.color(r * 0.8f * fLight, g * 0.8f * fLight, b * 0.8f * fLight);
		int texture = tile->getTexture(*m_pTileSource, pos, Facing::NORTH);
		renderNorth(tile, pos, texture);

		if (hasFancyGrass && texture == TEXTURE_GRASS_SIDE && this->m_fixedTexture < 0)
		{
			t.color(topR * 0.8f * fLight, topG * 0.8f * fLight, topB * 0.8f * fLight);
			renderNorth(tile, pos, TEXTURE_GRASS_SIDE_OVERLAY);
		}
	}

	if (m_bNoCulling || tile->shouldRenderFace(*m_pTileSource, pos.south(), Facing::SOUTH))
	{
		bDrewAnything = true;

		float fLight = tile->getBrightness(*m_pTileSource, pos.south());
		if (tile->m_aabb.max.z < 1.0f)
			fLight = fLightHere;

		t.color(r * 0.8f * fLight, g * 0.8f * fLight, b * 0.8f * fLight);
		int texture = tile->getTexture(*m_pTileSource, pos, Facing::SOUTH);
		renderSouth(tile, pos, texture);

		if (hasFancyGrass && texture == TEXTURE_GRASS_SIDE && this->m_fixedTexture < 0)
		{
			t.color(topR * 0.8f * fLight, topG * 0.8f * fLight, topB * 0.8f * fLight);
			renderSouth(tile, pos, TEXTURE_GRASS_SIDE_OVERLAY);
		}
	}

	if (m_bNoCulling || tile->shouldRenderFace(*m_pTileSource, pos.west(), Facing::WEST))
	{
		bDrewAnything = true;

		float fLight = tile->getBrightness(*m_pTileSource, pos.west());
		if (tile->m_aabb.min.x > 0.0f)
			fLight = fLightHere;

		t.color(r * 0.6f * fLight, g * 0.6f * fLight, b * 0.6f * fLight);
		int texture = tile->getTexture(*m_pTileSource, pos, Facing::WEST);
		renderWest(tile, pos, texture);

		if (hasFancyGrass && texture == TEXTURE_GRASS_SIDE && this->m_fixedTexture < 0)
		{
			t.color(topR * 0.6f * fLight, topG * 0.6f * fLight, topB * 0.6f * fLight);
			renderWest(tile, pos, TEXTURE_GRASS_SIDE_OVERLAY);
		}
	}

	if (m_bNoCulling || tile->shouldRenderFace(*m_pTileSource, pos.east(), Facing::EAST))
	{
		bDrewAnything = true;

		float fLight = tile->getBrightness(*m_pTileSource, pos.east());
		if (tile->m_aabb.max.x < 1.0f)
			fLight = fLightHere;

		t.color(r * 0.6f * fLight, g * 0.6f * fLight, b * 0.6f * fLight);
		int texture = tile->getTexture(*m_pTileSource, pos, Facing::EAST);
		renderEast(tile, pos, texture);

		if (hasFancyGrass && texture == TEXTURE_GRASS_SIDE && this->m_fixedTexture < 0)
		{
			t.color(topR * 0.6f * fLight, topG * 0.6f * fLight, topB * 0.6f * fLight);
			renderEast(tile, pos, TEXTURE_GRASS_SIDE_OVERLAY);
		}
	}

	return bDrewAnything;
}

bool TileRenderer::tesselateBlockInWorld(Tile* tile, const TilePos& pos)
{
	Color color = _getTileColor(pos, tile);
	color.a = 1.0f;

	if (useAmbientOcclusion())
	{
		if (Tile::lightEmission[tile->m_ID] == 0 /*&& Tile::translucency[tile->m_ID] < 0.9*/)
		{
			return tesselateBlockInWorldWithAmbienceOcclusionV2(tile, pos, color.r, color.g, color.b);
		}
	}

	return tesselateBlockInWorld(tile, pos, color.r, color.g, color.b);
}

bool TileRenderer::tesselateCrossInWorld(Tile* tile, const TilePos& pos)
{
	Tesselator& t = m_tessellator;

	Color color = _getTileColor(pos, tile);
	color.a = 1.0f;

	float bright = tile->getBrightness(*m_pTileSource, pos);
	color.mulRGB(bright);

	t.color(color);

	tesselateCrossTexture(FullTile(tile, m_pTileSource->getData(pos)), pos);

	return true;
}

bool TileRenderer::tesselateRowInWorld(Tile* tile, const TilePos& pos)
{
	Tesselator& t = m_tessellator;

	Color color = _getTileColor(pos, tile);
	color.a = 1.0f;
	t.color(color * ((0.85f * tile->getBrightness(*m_pTileSource, pos)) + 0.15f));

	tesselateRowTexture(tile, m_pTileSource->getData(pos), pos);

	return true;
}

bool TileRenderer::tesselateWaterInWorld(Tile* tile1, const TilePos& pos)
{
	constexpr float C_RATIO = 1.0f / 256.0f;

	LiquidTile* tile = (LiquidTile*)tile1;
	bool bRenderFaceDown, bRenderFaceUp, bRenderSides[4];

	Tesselator& t = m_tessellator;

	bRenderFaceDown = tile->shouldRenderFace(*m_pTileSource, pos.above(), Facing::UP);
	bRenderFaceUp   = tile->shouldRenderFace(*m_pTileSource, pos.below(), Facing::DOWN);

	bRenderSides[0] = tile->shouldRenderFace(*m_pTileSource, pos.north(), Facing::NORTH);
	bRenderSides[1] = tile->shouldRenderFace(*m_pTileSource, pos.south(), Facing::SOUTH);
	bRenderSides[2] = tile->shouldRenderFace(*m_pTileSource, pos.west(), Facing::WEST);
	bRenderSides[3] = tile->shouldRenderFace(*m_pTileSource, pos.east(), Facing::EAST);

	if (!bRenderFaceDown &&
		!bRenderFaceUp   &&
		!bRenderSides[0] &&
		!bRenderSides[1] &&
		!bRenderSides[2] &&
		!bRenderSides[3])
		return false;

	int tileData = m_pTileSource->getData(pos);
	float
		fHeight1 = getWaterHeight(pos, tile->m_pMaterial),
		fHeight2 = getWaterHeight(TilePos(pos.x, pos.y, pos.z + 1), tile->m_pMaterial),
		fHeight3 = getWaterHeight(TilePos(pos.x + 1, pos.y, pos.z + 1), tile->m_pMaterial),
		fHeight4 = getWaterHeight(pos.east(), tile->m_pMaterial);

	// @TODO: fix gotos
	bool bFlag1, bFlag2;
	if (!m_bNoCulling)
	{
		bFlag1 = bRenderFaceDown;
		if (!bRenderFaceDown)
		{
			if (!bRenderFaceUp)
				goto label_8;

			goto label_6;
		}
	}

	{
		// @NOTE: Have to use tile1 because for whatever reason MSVC doesn't think an overload
		// for `tile` exists that takes 2 int arguments
		int texFaceDown = tile->getTexture(Facing::UP, tileData);
		float slopeAngle = tile->getSlopeAngle(*m_pTileSource, pos, tile->m_pMaterial);

		int texX, texY;

		if (slopeAngle > -999.0f)
		{
			int texNorth = tile->getTexture(Facing::NORTH, tileData);
			texX = texNorth & 0xF0;
			texY = (texNorth & 0xF) * 16;
		}
		else
		{
			texX = texFaceDown & 0xF0;
			texY = (texFaceDown & 0xF) * 16;
		}

		float texUV_1, texUV_2, texUV_3, texUV_4, texUV_5, texUV_6, texUV_7, texUV_8;
		if (slopeAngle >= -999.0f)
		{
			texUV_1 = float(texY + 16) * C_RATIO;
			texUV_2 = float(texX + 16) * C_RATIO;
		}
		else
		{
			slopeAngle = 0.0f;
			texUV_1 = float(texY + 8) * C_RATIO;
			texUV_2 = float(texX + 8) * C_RATIO;
		}

		texUV_3 = C_RATIO * 8.0f * Mth::sin(slopeAngle);
		texUV_4 = C_RATIO * 8.0f * Mth::cos(slopeAngle);

		float bright = tile->getBrightness(*m_pTileSource, pos);

		texUV_5 = texUV_1 - texUV_4;
		texUV_6 = texUV_2 - texUV_4;

		t.color(bright, bright, bright);
		texUV_7 = texUV_2 + texUV_4;
		texUV_8 = texUV_1 + texUV_4;

		t.vertexUV(pos.x + 0.0f, pos.y + fHeight1, pos.z + 0.0f, (texUV_1 - texUV_4) - texUV_3, texUV_6 + texUV_3);
		t.vertexUV(pos.x + 0.0f, pos.y + fHeight2, pos.z + 1.0f, texUV_3 + texUV_5, texUV_7 + texUV_3);
		t.vertexUV(pos.x + 1.0f, pos.y + fHeight3, pos.z + 1.0f, texUV_8 + texUV_3, texUV_7 - texUV_3);
		t.vertexUV(pos.x + 1.0f, pos.y + fHeight4, pos.z + 0.0f, texUV_8 - texUV_3, texUV_6 - texUV_3);
	}

	if (m_bNoCulling)
		goto label_7;

	bFlag1 = true;
	if (bRenderFaceUp)
	{
	label_6:
	label_7:
		float bright = tile->getBrightness(*m_pTileSource, pos.below());
		t.color(bright * 0.5f, bright * 0.5f, bright * 0.5f);
		renderFaceDown(tile1, pos, tile->getTexture(Facing::DOWN));
		bFlag1 = true;
	}

label_8:
	bFlag2 = bFlag1;
	//bool bRenderedSides = false;

	for (int dir = 0; dir < 4; dir++)
	{
		int checkX = pos.x, checkZ = pos.z;
		switch (dir)
		{
			case 0: checkZ--; break;
			case 1: checkZ++; break;
			case 2: checkX--; break;
			case 3: checkX++; break;
		}

		int texture = tile1->getTexture((Facing::Name)(dir + Facing::NORTH), tileData);
		if (!m_bNoCulling && !bRenderSides[dir])
			continue;

		float vtxX1, vtxX2, vtxZ1, vtxZ2, height1, height2;
		switch (dir)
		{
			case 0:
			{
				vtxX1 = float(pos.x);
				vtxX2 = float(pos.x + 1);
				vtxZ1 = float(pos.z);
				vtxZ2 = float(pos.z);
				height1 = fHeight1;
				height2 = fHeight4;
				break;
			}
			case 1:
			{
				vtxX1 = float(pos.x + 1);
				vtxX2 = float(pos.x);
				vtxZ1 = float(pos.z + 1);
				vtxZ2 = float(pos.z + 1);
				height1 = fHeight3;
				height2 = fHeight2;
				break;
			}
			case 2:
			{
				vtxX1 = float(pos.x);
				vtxX2 = float(pos.x);
				vtxZ1 = float(pos.z + 1);
				vtxZ2 = float(pos.z);
				height1 = fHeight2;
				height2 = fHeight1;
				break;
			}
			case 3:
			{
				vtxX1 = float(pos.x + 1);
				vtxX2 = float(pos.x + 1);
				vtxZ1 = float(pos.z);
				vtxZ2 = float(pos.z + 1);
				height1 = fHeight4;
				height2 = fHeight3;
				break;
			}
		}

		float texU_1, texU_2, texV_1, texV_2, texV_3;

		int texX = (texture & 0xF) * 16;
		int texY = (texture >> 4) * 16;
		texU_1 = C_RATIO * float(texX);
		texU_2 = C_RATIO * (float(texX) + 15.99f);
		texV_1 = C_RATIO * (float(texY) + (1.0f - height1) * 16.0f);
		texV_2 = C_RATIO * (float(texY) + (1.0f - height2) * 16.0f);
		texV_3 = C_RATIO * (float(texY + 16.0f) - 0.01f);
		bFlag2 = true;
		//bRenderedSides = true;

		float brightMul = dir >= Facing::WEST ? 0.6f : 0.8f;
		float bright = tile->getBrightness(*m_pTileSource, TilePos(checkX, pos.y, checkZ));
		t.color(bright* brightMul, bright* brightMul, bright* brightMul);
		t.vertexUV(vtxX1, float(pos.y) + height1, vtxZ1, texU_1, texV_1);
		t.vertexUV(vtxX2, float(pos.y) + height2, vtxZ2, texU_2, texV_2);
		t.vertexUV(vtxX2, float(pos.y) + 0.0f, vtxZ2, texU_2, texV_3);
		t.vertexUV(vtxX1, float(pos.y) + 0.0f, vtxZ1, texU_1, texV_3);
	}

	tile->m_aabb.min.y = 0.0f;
	tile->m_aabb.max.y = 1.0f;

	return bFlag2;
}

#ifdef ENH_FIX_INVIS_STAIRS
#define PROC_RESULT(x) bRenderedAnything = (x) || bRenderedAnything;
#else
#define PROC_RESULT(x) (x)
#endif

bool TileRenderer::tesselateStairsInWorld(Tile* tile, const TilePos& pos)
{
	bool bRenderedAnything = false;

	switch (m_pTileSource->getData(pos))
	{
		case 0:
		{
			tile->setShape(0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 1.0f);
			PROC_RESULT(tesselateBlockInWorld(tile, pos));
			tile->setShape(0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
			PROC_RESULT(tesselateBlockInWorld(tile, pos));
			break;
		}
		case 1:
		{
			tile->setShape(0.0f, 0.0f, 0.0f, 0.5f, 1.0f, 1.0f);
			PROC_RESULT(tesselateBlockInWorld(tile, pos));
			tile->setShape(0.5f, 0.0f, 0.0f, 1.0f, 0.5f, 1.0f);
			PROC_RESULT(tesselateBlockInWorld(tile, pos));
			break;
		}
		case 2:
		{
			tile->setShape(0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f);
			PROC_RESULT(tesselateBlockInWorld(tile, pos));
			tile->setShape(0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f);
			PROC_RESULT(tesselateBlockInWorld(tile, pos));
			break;
		}
		case 3:
		{
			tile->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f);
			PROC_RESULT(tesselateBlockInWorld(tile, pos));
			tile->setShape(0.0f, 0.0f, 0.5f, 1.0f, 0.5f, 1.0f);
			PROC_RESULT(tesselateBlockInWorld(tile, pos));
			break;
		}
	}

	tile->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	// @BUG: In a chunk that only contains stairs, everything will be invisible, because this
	// function returns 0.
	return bRenderedAnything;
}

bool TileRenderer::tesselateFenceInWorld(Tile* tile, const TilePos& pos)
{
	tile->setShape(0.375f, 0.0f, 0.375f, 0.625f, 1.0f, 0.625f);
	bool bRenderedAnything = tesselateBlockInWorld(tile, pos);

	FenceTile* pFenceTile = (FenceTile*)Tile::fence;

	bool tileWest = pFenceTile->shouldConnectTo(*m_pTileSource, pos.west());
	bool tileEast = pFenceTile->shouldConnectTo(*m_pTileSource, pos.east());
	bool tileNorth = pFenceTile->shouldConnectTo(*m_pTileSource, pos.north());
	bool tileSouth = pFenceTile->shouldConnectTo(*m_pTileSource, pos.south());

	bool connectsHorizontally = tileWest || tileEast;
	bool connectsVertically = tileNorth || tileSouth;

	if (!connectsHorizontally && !connectsVertically)
	{
		connectsHorizontally = true;
	}

	float var6 = 7.0f / 16.0f;
	float var7 = 9.0f / 16.0f;
	float var14 = 12.0f / 16.0f;
	float var15 = 15.0f / 16.0f;
	float var16 = tileWest ? 0.0f : var6;
	float var17 = tileEast ? 1.0f : var7;
	float var18 = tileNorth ? 0.0f : var6;
	float var19 = tileSouth ? 1.0f : var7;

	if (connectsHorizontally)
	{
		tile->setShape(var16, var14, var6, var17, var15, var7);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;

	}

	if (connectsVertically)
	{
		tile->setShape(var6, var14, var18, var7, var15, var19);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
	}

	var14 = 6.0f / 16.0f;
	var15 = 9.0f / 16.0f;

	if (connectsHorizontally)
	{
		tile->setShape(var16, var14, var6, var17, var15, var7);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
	}

	if (connectsVertically)
	{
		tile->setShape(var6, var14, var18, var7, var15, var19);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
	}

	tile->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	return bRenderedAnything;
}

bool TileRenderer::tesselateFenceGateInWorld(Tile* tile, const TilePos& pos)
{
	int metadata = m_pTileSource->getData(pos);
	int getOpen = metadata & 4;
	int getFacing = metadata & 3;
	bool bRenderedAnything = false;

	constexpr float o = 0.4375f;
	constexpr float q = 0.5625f;
	float g = 0.0f;
	float m = 0.125f;
	float var22 = 0.375f;
	float var29 = 0.5f;

	if (getFacing != 3 && getFacing != 1) 
	{
		tile->setShape(g, 0.3125f, o, m, 1.0f, q);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		g = 0.875f;
		m = 1.0f;
		tile->setShape(g, 0.3125f, o, m, 1.0f, q);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
	}
	else 
	{
		tile->setShape(o, 0.3125f, g, q, 1.0f, m);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		g = 0.875f;
		m = 1.0f;
		tile->setShape(o, 0.3125f, g, q, 1.0f, m);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
	}

	if (getOpen == 0) {
		if (getFacing != 3 && getFacing != 1) {
			tile->setShape(var22, 0.375f, o, var29, 0.9375f, q);
			if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
			var22 = 0.5f;
			var29 = 0.625f;
			tile->setShape(var22, 0.375f, o, var29, 0.9375f, q);
			if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
			var22 = 0.625f;
			var29 = 0.875f;
			tile->setShape(var22, 0.375f, o, var29, 0.5625f, q);
			if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
			tile->setShape(var22, 0.75f, o, var29, 0.9375f, q);
			if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
			var22 = 0.125f;
			var29 = 0.375f;
			tile->setShape(var22, 0.375f, o, var29, 0.5625f, q);
			if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
			tile->setShape(var22, 0.75f, o, var29, 0.9375f, q);
			if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		}
		else {
			tile->setShape(o, 0.375f, var22, q, 0.9375f, var29);
			if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
			var22 = 0.5f;
			var29 = 0.625f;
			tile->setShape(o, 0.375f, var22, q, 0.9375f, var29);
			if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
			var22 = 0.625f;
			var29 = 0.875f;
			tile->setShape(o, 0.375f, var22, q, 0.5625f, var29);
			if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
			tile->setShape(o, 0.75f, var22, q, 0.9375f, var29);
			if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
			var22 = 0.125f;
			var29 = 0.375f;
			tile->setShape(o, 0.375f, var22, q, 0.5625f, var29);
			if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
			tile->setShape(o, 0.75f, var22, q, 0.9375f, var29);
			if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		}
	}
	else if (getFacing == 3) {
		tile->setShape(0.8125f, 0.375f, 0.0f, 0.9375f, 0.9375f, 0.125f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		tile->setShape(0.8125f, 0.375f, 0.875f, 0.9375f, 0.9375f, 1.0f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		tile->setShape(0.5625f, 0.375f, 0.0f, 0.8125f, 0.5625f, 0.125f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		tile->setShape(0.5625f, 0.375f, 0.875f, 0.8125f, 0.5625f, 1.0f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		tile->setShape(0.5625f, 0.75f, 0.0f, 0.8125f, 0.9375f, 0.125f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		tile->setShape(0.5625f, 0.75f, 0.875f, 0.8125f, 0.9375f, 1.0f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
	}
	else if (getFacing == 1) {
		tile->setShape(0.0625f, 0.375f, 0.0f, 0.1875f, 0.9375f, 0.125f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		tile->setShape(0.0625f, 0.375f, 0.875f, 0.1875f, 0.9375f, 1.0f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		tile->setShape(0.1875f, 0.375f, 0.0f, 0.4375f, 0.5625f, 0.125f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		tile->setShape(0.1875f, 0.375f, 0.875f, 0.4375f, 0.5625f, 1.0f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		tile->setShape(0.1875f, 0.75f, 0.0f, 0.4375f, 0.9375f, 0.125f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		tile->setShape(0.1875f, 0.75f, 0.875f, 0.4375f, 0.9375f, 1.0f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
	}
	else if (getFacing == 0) {
		tile->setShape(0.0f, 0.375f, 0.8125f, 0.125f, 0.9375f, 0.9375f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		tile->setShape(0.875f, 0.375f, 0.8125f, 1.0f, 0.9375f, 0.9375f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		tile->setShape(0.0f, 0.375f, 0.5625f, 0.125f, 0.5625f, 0.8125f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		tile->setShape(0.875f, 0.375f, 0.5625f, 1.0f, 0.5625f, 0.8125f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		tile->setShape(0.0f, 0.75f, 0.5625f, 0.125f, 0.9375f, 0.8125f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		tile->setShape(0.875f, 0.75f, 0.5625f, 1.0f, 0.9375f, 0.8125f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
	}
	else if (getFacing == 2) {
		tile->setShape(0.0f, 0.375f, 0.0625f, 0.125f, 0.9375f, 0.1875f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		tile->setShape(0.875f, 0.375f, 0.0625f, 1.0f, 0.9375f, 0.1875f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		tile->setShape(0.0f, 0.375f, 0.1875f, 0.125f, 0.5625f, 0.4375f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		tile->setShape(0.875f, 0.375f, 0.1875f, 1.0f, 0.5625f, 0.4375f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		tile->setShape(0.0f, 0.75f, 0.1875f, 0.125f, 0.9375f, 0.4375f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
		tile->setShape(0.875f, 0.75f, 0.1875f, 1.0f, 0.9375f, 0.4375f);
		if (tesselateBlockInWorld(tile, pos)) bRenderedAnything = true;
	}

	tile->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	return bRenderedAnything;
}

bool TileRenderer::tesselateDoorInWorld(Tile* tile, const TilePos& pos)
{
	Tesselator& t = m_tessellator;
	float fBrightHere = tile->getBrightness(*m_pTileSource, pos), fBright;
	int texture;

	fBright = tile->getBrightness(*m_pTileSource, pos.below());
	if (tile->m_aabb.min.y > 0.0f)       fBright = fBrightHere;
	if (Tile::lightEmission[tile->m_ID]) fBright = 1.0f;
	t.color(fBright * 0.5f, fBright * 0.5f, fBright * 0.5f);
	renderFaceDown(tile, pos, tile->getTexture(*m_pTileSource, pos, Facing::DOWN));

	fBright = tile->getBrightness(*m_pTileSource, pos.above());
	if (tile->m_aabb.max.y < 1.0f)       fBright = fBrightHere;
	if (Tile::lightEmission[tile->m_ID]) fBright = 1.0f;
	t.color(fBright, fBright, fBright);
	renderFaceUp(tile, pos, tile->getTexture(*m_pTileSource, pos, Facing::UP));

	fBright = tile->getBrightness(*m_pTileSource, pos.north());
	if (tile->m_aabb.min.z > 0.0f)       fBright = fBrightHere;
	if (Tile::lightEmission[tile->m_ID]) fBright = 1.0f;
	t.color(fBright * 0.8f, fBright * 0.8f, fBright * 0.8f);
	texture = tile->getTexture(*m_pTileSource, pos, Facing::NORTH);
	if (texture < 0) texture = -texture, m_bXFlipTexture = true;
	renderNorth(tile, pos, texture);
	m_bXFlipTexture = false;

	fBright = tile->getBrightness(*m_pTileSource, pos.south());
	if (tile->m_aabb.max.z < 1.0f)       fBright = fBrightHere;
	if (Tile::lightEmission[tile->m_ID]) fBright = 1.0f;
	t.color(fBright * 0.8f, fBright * 0.8f, fBright * 0.8f);
	texture = tile->getTexture(*m_pTileSource, pos, Facing::SOUTH);
	if (texture < 0) texture = -texture, m_bXFlipTexture = true;
	renderSouth(tile, pos, texture);
	m_bXFlipTexture = false;

	fBright = tile->getBrightness(*m_pTileSource,pos.west());
	if (tile->m_aabb.min.x > 0.0f)       fBright = fBrightHere;
	if (Tile::lightEmission[tile->m_ID]) fBright = 1.0f;
	t.color(fBright * 0.6f, fBright * 0.6f, fBright * 0.6f);
	texture = tile->getTexture(*m_pTileSource, pos, Facing::WEST);
	if (texture < 0) texture = -texture, m_bXFlipTexture = true;
	renderWest(tile, pos, texture);
	m_bXFlipTexture = false;

	fBright = tile->getBrightness(*m_pTileSource, pos.east());
	if (tile->m_aabb.max.x < 1.0f)       fBright = fBrightHere;
	if (Tile::lightEmission[tile->m_ID]) fBright = 1.0f;
	t.color(fBright * 0.6f, fBright * 0.6f, fBright * 0.6f);
	texture = tile->getTexture(*m_pTileSource, pos, Facing::EAST);
	if (texture < 0) texture = -texture, m_bXFlipTexture = true;
	renderEast(tile, pos, texture);
	m_bXFlipTexture = false;

	return true;
}

void TileRenderer::tesselateTorch(Tile* tile, const Vec3& pos, float a, float b)
{
	constexpr float C_RATIO = 1.0f / 256.0f;
	constexpr float C_ONE_PIXEL = 1.0f / 16.0f;
	constexpr float C_HALF_TILE = 1.0f / 2.0f;
	constexpr float C_TOP_SKEW_RATIO = 0.375f;

	int texture = tile->getTexture(Facing::DOWN);

	if (m_fixedTexture >= 0)
		texture = m_fixedTexture;

	// @TODO: Clean up a bit more

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	float texU_1 = texX * C_RATIO;
	float texU_2 = (texX + 15.99f) * C_RATIO;
	float texV_1 = (texY * C_RATIO) + 1.0f / 32.0f;
	float texV_2 = (texY + 15.99f) * C_RATIO;

	float x1 = pos.x + C_HALF_TILE, z1 = pos.z + C_HALF_TILE;
	float x2 = x1 + (float)(a * C_TOP_SKEW_RATIO);
	float z2 = z1 + (float)(b * C_TOP_SKEW_RATIO);

	Tesselator& t = m_tessellator;
	
	// Top side (flame)
	float x_1 = x2 - C_ONE_PIXEL; 
	float x_2 = x2 + C_ONE_PIXEL;
	float z_1 = z2 - C_ONE_PIXEL;
	float z_2 = z2 + C_ONE_PIXEL;

	// Edges (close by)
	float x_3 = x1 - C_ONE_PIXEL;
	float x_4 = x1 + C_ONE_PIXEL;
	float z_5 = z1 - C_ONE_PIXEL;
	float z_6 = z1 + C_ONE_PIXEL;
	
	// Far edges
	float x_5 = x1 - C_HALF_TILE; 
	float x_6 = x1 + C_HALF_TILE;
	float z_3 = z1 - C_HALF_TILE;
	float z_4 = z1 + C_HALF_TILE;
	
	float x_7 = x_6 + a; // Skewed bottom
	float x_8 = x_3 + a;
	float x_9 = x_4 + a;
	float x_0 = x_5 + a;
	float z_7 = z_3 + b;
	float z_8 = z_4 + b;
	float z_9 = z_5 + b;
	float z_0 = z_6 + b;

	float y_1 = pos.y + C_ONE_PIXEL * 10.0f; // Torch height
	float y_2 = pos.y + 1.0f; // Top
	float y_3 = pos.y + 0.0f; // Bottom

	float texU_3 = texU_1 + 0.027344f;
	float texU_4 = texU_1 + 0.035156f;
	float texV_3 = texY * C_RATIO;
	float texV_4 = texY * C_RATIO + 0.023438f;

	// Top
	t.vertexUV(x_1, y_1, z_1, texU_3, texV_4);
	t.vertexUV(x_1, y_1, z_2, texU_3, texV_1);
	t.vertexUV(x_2, y_1, z_2, texU_4, texV_1);
	t.vertexUV(x_2, y_1, z_1, texU_4, texV_4);
	// Sides
	t.vertexUV(x_3, y_2, z_3, texU_1, texV_3);
	t.vertexUV(x_8, y_3, z_7, texU_1, texV_2);
	t.vertexUV(x_8, y_3, z_8, texU_2, texV_2);
	t.vertexUV(x_3, y_2, z_4, texU_2, texV_3);
	
	t.vertexUV(x_4, y_2, z_4, texU_1, texV_3);
	t.vertexUV(x_9, y_3, z_8, texU_1, texV_2);
	t.vertexUV(x_9, y_3, z_7, texU_2, texV_2);
	t.vertexUV(x_4, y_2, z_3, texU_2, texV_3);
	
	t.vertexUV(x_5, y_2, z_6, texU_1, texV_3);
	t.vertexUV(x_0, y_3, z_0, texU_1, texV_2);
	t.vertexUV(x_7, y_3, z_0, texU_2, texV_2);
	t.vertexUV(x_6, y_2, z_6, texU_2, texV_3);
	
	t.vertexUV(x_6, y_2, z_5, texU_1, texV_3);
	t.vertexUV(x_7, y_3, z_9, texU_1, texV_2);
	t.vertexUV(x_0, y_3, z_9, texU_2, texV_2);
	t.vertexUV(x_5, y_2, z_5, texU_2, texV_3);
}

bool TileRenderer::tesselateTorchInWorld(Tile* tile, const TilePos& pos)
{
	TileData data = m_pTileSource->getData(pos);
	float bright = tile->getBrightness(*m_pTileSource, pos);

	if (Tile::lightEmission[tile->m_ID] > 0)
		bright = 1.0f;

	Tesselator& t = m_tessellator;
	t.color(bright, bright, bright);

	switch (data)
	{
		case 1:
			tesselateTorch(tile, Vec3(float(pos.x) - 0.1f, float(pos.y) + 0.2f, float(pos.z)), -0.4f, 0.0f);
			break;
		case 2:
			tesselateTorch(tile, Vec3(float(pos.x) + 0.1f, float(pos.y) + 0.2f, float(pos.z)), 0.4f, 0.0f);
			break;
		case 3:
			tesselateTorch(tile, Vec3(float(pos.x), float(pos.y) + 0.2f, float(pos.z) - 0.1f), 0.0f, -0.4f);
			break;
		case 4:
			tesselateTorch(tile, Vec3(float(pos.x), float(pos.y) + 0.2f, float(pos.z) + 0.1f), 0.0f, 0.4f);
			break;
		default:
			tesselateTorch(tile, pos, 0.0f, 0.0f);
			break;
	}

	return true;
}

bool TileRenderer::tesselateDiodeInWorld(Tile* tile, const TilePos& pos)
{
	int data = m_pTileSource->getData(pos); // var5
	int dir = data & 3; // var6
	int flipped = (data & 12) >> 2; // var7
	tesselateBlockInWorld(tile, pos);
	Tesselator& t = Tesselator::instance; // var8
	float br = tile->getBrightness(*m_pTileSource, pos); // var9
	if (Tile::lightEmission[tile->m_ID] > 0)
	{
		br = (br + 1.0f) * 0.5f;
	}

	t.color(br, br, br);
	float var10 = -0.1875f;
	float var12 = 0.0f;
	float var14 = 0.0f;
	float var16 = 0.0f;
	float var18 = 0.0f;
	switch (dir)
	{
	case 0:
		var18 = -0.3125f;
		var14 = DiodeTile::unk_a[flipped];
		break;
	case 1:
		var16 = 0.3125f;
		var12 = -DiodeTile::unk_a[flipped];
		break;
	case 2:
		var18 = 0.3125f;
		var14 = -DiodeTile::unk_a[flipped];
		break;
	case 3:
		var16 = -0.3125f;
		var12 = DiodeTile::unk_a[flipped];
	}

	tesselateTorch(tile, Vec3(float(pos.x) + var12, float(pos.y) + var10, float(pos.z) + var14), 0.0f, 0.0f);
	tesselateTorch(tile, Vec3(float(pos.x) + var16, float(pos.y) + var10, float(pos.z) + var18), 0.0f, 0.0f);
	int var20 = tile->getTexture(Facing::UP);
	int var21 = (var20 & 15) << 4;
	int var22 = var20 & 240;
	float var23 = float(var21) / 256.0f;
	float var25 = (float(var21) + 15.99f) / 256.0f;
	float var27 = float(var22) / 256.0f;
	float var29 = (float(var22) + 15.99f) / 256.0f;
	float var31 = 2.0f / 16.0f;
	/*
	float var32 = float(pos.x + 1);
	float var33 = float(pos.x + 1);
	float var34 = float(pos.x + 0);
	float var35 = float(pos.x + 0);
	float var36 = float(pos.z + 0);
	float var37 = float(pos.z + 1);
	float var38 = float(pos.z + 1);
	float var39 = float(pos.z + 0);
	*/
	float var40 = float(pos.y) + var31;
	TilePos pos1;
	TilePos pos2;
	TilePos pos3;
	TilePos pos4;
	switch (dir)
	{
	case 1:
		/*
		var35 = float(pos.x + 1);
		var32 = var35;
		var34 = float(pos.x + 0);
		var33 = var34;
		var37 = float(pos.z + 1);
		var36 = var37;
		var39 = float(pos.z + 0);
		var38 = var39;
		*/
		pos1 = pos.east();
		pos2 = pos;
		pos3 = pos.south();
		pos4 = pos.east().south();
		break;
	case 2:
		/*
		var33 = float(pos.x + 0);
		var32 = var33;
		var35 = float(pos.x + 1);
		var34 = var35;
		var39 = float(pos.z + 1);
		var36 = var39;
		var38 = float(pos.z + 0);
		var37 = var38;
		*/
		pos1 = pos.east().south();
		pos2 = pos.east();
		pos3 = pos;
		pos4 = pos.south();
		break;
	case 3:
		/*
		var35 = float(pos.x + 0);
		var32 = var35;
		var34 = float(pos.x + 1);
		var33 = var34;
		var37 = float(pos.z + 0);
		var36 = var37;
		var39 = float(pos.z + 1);
		var38 = var39;
		*/
		pos1 = pos.south();
		pos2 = pos.east().south();
		pos3 = pos.east();
		pos4 = pos;
		break;
	default:
		pos1 = pos;
		pos2 = pos.south();
		pos3 = pos.east().south();
		pos4 = pos.east();
		break;
	}

	t.vertexUV(float(pos1.x), var40, float(pos1.z), var23, var27);
	t.vertexUV(float(pos2.x), var40, float(pos2.z), var23, var29);
	t.vertexUV(float(pos3.x), var40, float(pos3.z), var25, var29);
	t.vertexUV(float(pos4.x), var40, float(pos4.z), var25, var27);
	return true;
}

bool TileRenderer::tesselateLeverInWorld(Tile* tile, const TilePos& pos)
{
	TileData data = m_pTileSource->getData(pos);
	int dir = data & 7;
	bool flipped = (data & 8) > 0;
	Tesselator& t = Tesselator::instance;
	bool hadFixed = m_fixedTexture >= 0;
	if (!hadFixed)
	{
		m_fixedTexture = Tile::stoneBrick->m_TextureFrame;
	}

	constexpr float w1 = 0.25f;
	constexpr float w2 = 0.1875f;
	constexpr float h = 0.1875f;
	switch (dir)
	{
	case 1:
		tile->setShape(0.0f, 0.5f - w1, 0.5f - w2, h, 0.5f + w1, 0.5f + w2);
		break;
	case 2:
		tile->setShape(1.0f - h, 0.5f - w1, 0.5f - w2, 1.0f, 0.5f + w1, 0.5f + w2);
		break;
	case 3:
		tile->setShape(0.5f - w2, 0.5f - w1, 0.0f, 0.5f + w2, 0.5f + w1, h);
		break;
	case 4:
		tile->setShape(0.5f - w2, 0.5f - w1, 1.0f - h, 0.5f + w2, 0.5f + w1, 1.0f);
		break;
	case 5:
		tile->setShape(0.5f - w2, 0.0f, 0.5f - w1, 0.5f + w2, h, 0.5f + w1);
		break;
	case 6:
		tile->setShape(0.5f - w1, 0.0f, 0.5f - w2, 0.5f + w1, h, 0.5f + w2);
		break;
	default:
		break;
	}

	tesselateBlockInWorld(tile, pos);
	if (!hadFixed)
	{
		m_fixedTexture = -1;
	}

	float br = tile->getBrightness(*m_pTileSource, pos);
	if (Tile::lightEmission[tile->m_ID] > 0)
	{
		br = 1.0f;
	}

	t.color(br, br, br);
	int tex = tile->getTexture(Facing::DOWN);
	if (m_fixedTexture >= 0)
	{
		tex = m_fixedTexture;
	}

	int xt = (tex & 15) << 4;
	int yt = tex & 240;
	float u0 = (float)xt / 256.0f;
	float u1 = ((float)xt + 15.99f) / 256.0f;
	float v0 = (float)yt / 256.0f;
	float v1 = ((float)yt + 15.99f) / 256.0f;
	constexpr float xv = 0.0625f;
	constexpr float zv = 0.0625f;
	constexpr float yv = 0.625f;
	Vec3 corners[8] = {
		Vec3(-xv, 0.0f, -zv),
		Vec3( xv, 0.0f, -zv),
		Vec3( xv, 0.0f,  zv),
		Vec3(-xv, 0.0f,  zv),
		Vec3(-xv, yv,   -zv),
		Vec3( xv, yv,   -zv),
		Vec3( xv, yv,    zv),
		Vec3(-xv, yv,    zv)
	};

	for (int i = 0; i < 8; ++i) {
		if (flipped)
		{
			corners[i].z -= 0.0625f;
			corners[i].xRot(float(M_PI) * 2.0f / 9.0f);
		}
		else
		{
			corners[i].z += 0.0625f;
			corners[i].xRot(float(-M_PI) * 2.0f / 9.0f);
		}

		if (dir == 6)
		{
			corners[i].yRot(float(M_PI / 2));
		}

		if (dir < 5)
		{
			corners[i].y -= 0.375;
			corners[i].xRot(float(M_PI / 2));
			switch (dir)
			{
			case 1:
				corners[i].yRot(float(-M_PI / 2));
				break;
			case 2:
				corners[i].yRot(float(M_PI / 2));
				break;
			case 3:
				corners[i].yRot(float(M_PI));
				break;
			case 4:
				corners[i].yRot(0.0f);
				break;
			}

			corners[i] += pos + 0.5f;
		}
		else
		{
			corners[i].x += pos.x + 0.5f;
			corners[i].y += pos.y + 0.125f;
			corners[i].z += pos.z + 0.5f;
		}
	}

	Vec3 c0;
	Vec3 c1;
	Vec3 c2;
	Vec3 c3;

	for (int i = 0; i < 6; ++i)
	{
		if (i == 0)
		{
			u0 = float(xt + 7) / 256.0f;
			u1 = (float(xt + 9) - 0.01f) / 256.0f;
			v0 = float(yt + 6) / 256.0f;
			v1 = (float(yt + 8) - 0.01f) / 256.0f;
		}
		else if (i == 2)
		{
			u0 = float(xt + 7) / 256.0f;
			u1 = (float(xt + 9) - 0.01f) / 256.0f;
			v0 = float(yt + 6) / 256.0f;
			v1 = (float(yt + 16) - 0.01f) / 256.0f;
		}

		switch (i)
		{
		case 0:
			c0 = corners[0];
			c1 = corners[1];
			c2 = corners[2];
			c3 = corners[3];
			break;
		case 1:
			c0 = corners[7];
			c1 = corners[6];
			c2 = corners[5];
			c3 = corners[4];
			break;
		case 2:
			c0 = corners[1];
			c1 = corners[0];
			c2 = corners[4];
			c3 = corners[5];
			break;
		case 3:
			c0 = corners[2];
			c1 = corners[1];
			c2 = corners[5];
			c3 = corners[6];
			break;
		case 4:
			c0 = corners[3];
			c1 = corners[2];
			c2 = corners[6];
			c3 = corners[7];
			break;
		case 5:
			c0 = corners[0];
			c1 = corners[3];
			c2 = corners[7];
			c3 = corners[4];
			break;
		}

		t.vertexUV(c0.x, c0.y, c0.z, u0, v1);
		t.vertexUV(c1.x, c1.y, c1.z, u1, v1);
		t.vertexUV(c2.x, c2.y, c2.z, u1, v0);
		t.vertexUV(c3.x, c3.y, c3.z, u0, v0);
	}

	return true;
}

bool TileRenderer::tesselateLadderInWorld(Tile* tile, const TilePos& pos)
{
	constexpr float C_RATIO = 1.0f / 256.0f;

	Tesselator& t = m_tessellator;

	int texture = tile->getTexture(Facing::DOWN);

	if (m_fixedTexture >= 0)
		texture = m_fixedTexture;

	float bright = m_pTileSource->getBrightness(pos);
	t.color(bright, bright, bright);

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	float texU_1 = C_RATIO * texX;
	float texU_2 = C_RATIO * (texX + 15.99f);
	float texV_1 = C_RATIO * texY;
	float texV_2 = C_RATIO * (texY + 15.99f);

	switch (m_pTileSource->getData(pos))
	{
		case 2:
			t.vertexUV(float(pos.x + 1), float(pos.y + 1), float(pos.z + 1) - 0.05f, texU_1, texV_1);
			t.vertexUV(float(pos.x + 1), float(pos.y + 0), float(pos.z + 1) - 0.05f, texU_1, texV_2);
			t.vertexUV(float(pos.x + 0), float(pos.y + 0), float(pos.z + 1) - 0.05f, texU_2, texV_2);
			t.vertexUV(float(pos.x + 0), float(pos.y + 1), float(pos.z + 1) - 0.05f, texU_2, texV_1);
			break;
		case 3:
			t.vertexUV(float(pos.x + 1), float(pos.y + 0), float(pos.z) + 0.05f, texU_2, texV_2);
			t.vertexUV(float(pos.x + 1), float(pos.y + 1), float(pos.z) + 0.05f, texU_2, texV_1);
			t.vertexUV(float(pos.x + 0), float(pos.y + 1), float(pos.z) + 0.05f, texU_1, texV_1);
			t.vertexUV(float(pos.x + 0), float(pos.y + 0), float(pos.z) + 0.05f, texU_1, texV_2);
			break;
		case 4:
			t.vertexUV(float(pos.x + 1) - 0.05f, float(pos.y + 0), float(pos.z + 1), texU_2, texV_2);
			t.vertexUV(float(pos.x + 1) - 0.05f, float(pos.y + 1), float(pos.z + 1), texU_2, texV_1);
			t.vertexUV(float(pos.x + 1) - 0.05f, float(pos.y + 1), float(pos.z + 0), texU_1, texV_1);
			t.vertexUV(float(pos.x + 1) - 0.05f, float(pos.y + 0), float(pos.z + 0), texU_1, texV_2);
			break;
		case 5:
			t.vertexUV(float(pos.x) + 0.05f, float(pos.y + 1), float(pos.z + 1), texU_1, texV_1);
			t.vertexUV(float(pos.x) + 0.05f, float(pos.y + 0), float(pos.z + 1), texU_1, texV_2);
			t.vertexUV(float(pos.x) + 0.05f, float(pos.y + 0), float(pos.z + 0), texU_2, texV_2);
			t.vertexUV(float(pos.x) + 0.05f, float(pos.y + 1), float(pos.z + 0), texU_2, texV_1);
			break;
	}

	return true;
}

bool TileRenderer::tesselateFireInWorld(Tile* tile, const TilePos& pos)
{
	constexpr float C_RATIO = 1.0f / 256.0f;

	Tesselator& t = m_tessellator;

	int texture = tile->getTexture(Facing::DOWN);
	float bright = tile->getBrightness(*m_pTileSource, pos);

	t.color(bright, bright, bright);

	FireTile* pFireTile = (FireTile*)Tile::fire;

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	float texU_1 = C_RATIO * (texX);
	float texU_2 = C_RATIO * (texX + 15.99f);
	float texV_1 = C_RATIO * (texY);
	float texV_2 = C_RATIO * (texY + 15.99f);
	float xf = float(pos.x), yf = float(pos.y), zf = float(pos.z);

	if (m_pTileSource->isSolidBlockingTile(pos.below()) || pFireTile->canBurn(*m_pTileSource, pos.below()))
	{
		t.vertexUV(xf + 0.5f - 0.3f, yf + 1.4f, zf + 1.0f, texU_2, texV_1);
		t.vertexUV(xf + 0.5f + 0.2f, yf + 0.0f, zf + 1.0f, texU_2, texV_2);
		t.vertexUV(xf + 0.5f + 0.2f, yf + 0.0f, zf + 0.0f, texU_1, texV_2);
		t.vertexUV(xf + 0.5f - 0.3f, yf + 1.4f, zf + 0.0f, texU_1, texV_1);

		t.vertexUV(xf + 0.5f + 0.3f, yf + 1.4f, zf + 0.0f, texU_2, texV_1);
		t.vertexUV(xf + 0.5f - 0.2f, yf + 0.0f, zf + 0.0f, texU_2, texV_2);
		t.vertexUV(xf + 0.5f - 0.2f, yf + 0.0f, zf + 1.0f, texU_1, texV_2);
		t.vertexUV(xf + 0.5f + 0.3f, yf + 1.4f, zf + 1.0f, texU_1, texV_1);

		t.vertexUV(xf + 1.0f, yf + 1.4f, zf + 0.5f + 0.3f, texU_2, texV_1);
		t.vertexUV(xf + 1.0f, yf + 0.0f, zf + 0.5f - 0.2f, texU_2, texV_2);
		t.vertexUV(xf + 0.0f, yf + 0.0f, zf + 0.5f - 0.2f, texU_1, texV_2);
		t.vertexUV(xf + 0.0f, yf + 1.4f, zf + 0.5f + 0.3f, texU_1, texV_1);

		t.vertexUV(xf + 0.0f, yf + 1.4f, zf + 0.5f - 0.3f, texU_2, texV_1);
		t.vertexUV(xf + 0.0f, yf + 0.0f, zf + 0.5f + 0.2f, texU_2, texV_2);
		t.vertexUV(xf + 1.0f, yf + 0.0f, zf + 0.5f + 0.2f, texU_1, texV_2);
		t.vertexUV(xf + 1.0f, yf + 1.4f, zf + 0.5f - 0.3f, texU_1, texV_1);

		t.vertexUV(xf + 0.5f - 0.4f, yf + 1.4f, zf + 0.0f, texU_1, texV_1);
		t.vertexUV(xf + 0.5f - 0.5f, yf + 0.0f, zf + 0.0f, texU_1, texV_2);
		t.vertexUV(xf + 0.5f - 0.5f, yf + 0.0f, zf + 1.0f, texU_2, texV_2);
		t.vertexUV(xf + 0.5f - 0.4f, yf + 1.4f, zf + 1.0f, texU_2, texV_1);

		t.vertexUV(xf + 0.5f + 0.4f, yf + 1.4f, zf + 1.0f, texU_1, texV_1);
		t.vertexUV(xf + 0.5f + 0.5f, yf + 0.0f, zf + 1.0f, texU_1, texV_2);
		t.vertexUV(xf + 0.5f + 0.5f, yf + 0.0f, zf + 0.0f, texU_2, texV_2);
		t.vertexUV(xf + 0.5f + 0.4f, yf + 1.4f, zf + 0.0f, texU_2, texV_1);

		t.vertexUV(xf + 0.0f, yf + 1.4f, zf + 0.5f + 0.4f, texU_1, texV_1);
		t.vertexUV(xf + 0.0f, yf + 0.0f, zf + 0.5f + 0.5f, texU_1, texV_2);
		t.vertexUV(xf + 1.0f, yf + 0.0f, zf + 0.5f + 0.5f, texU_2, texV_2);
		t.vertexUV(xf + 1.0f, yf + 1.4f, zf + 0.5f + 0.4f, texU_2, texV_1);

		t.vertexUV(xf + 1.0f, yf + 1.4f, zf + 0.5f - 0.4f, texU_1, texV_1);
		t.vertexUV(xf + 1.0f, yf + 0.0f, zf + 0.5f - 0.5f, texU_1, texV_2);
		t.vertexUV(xf + 0.0f, yf + 0.0f, zf + 0.5f - 0.5f, texU_2, texV_2);
		t.vertexUV(xf + 0.0f, yf + 1.4f, zf + 0.5f - 0.4f, texU_2, texV_1);

		return true;
	}

	if (((pos.y / 2 + pos.x / 2 + pos.z / 2) & 1) == 0)
		std::swap(texU_1, texU_2);

	if (pFireTile->canBurn(*m_pTileSource, pos.west()))
	{
		t.vertexUV(xf + 0.2f, (yf + 1.4f) + 0.0625f, zf + 1.0f, texU_1, texV_1);
		t.vertexUV(xf + 0.0f, (yf + 0.0f) + 0.0625f, zf + 1.0f, texU_1, texV_2);
		t.vertexUV(xf + 0.0f, (yf + 0.0f) + 0.0625f, zf + 0.0f, texU_2, texV_2);
		t.vertexUV(xf + 0.2f, (yf + 1.4f) + 0.0625f, zf + 0.0f, texU_2, texV_1);
		t.vertexUV(xf + 0.2f, (yf + 1.4f) + 0.0625f, zf + 0.0f, texU_2, texV_1);
		t.vertexUV(xf + 0.0f, (yf + 0.0f) + 0.0625f, zf + 0.0f, texU_2, texV_2);
		t.vertexUV(xf + 0.0f, (yf + 0.0f) + 0.0625f, zf + 1.0f, texU_1, texV_2);
		t.vertexUV(xf + 0.2f, (yf + 1.4f) + 0.0625f, zf + 1.0f, texU_1, texV_1);
	}

	if (pFireTile->canBurn(*m_pTileSource, pos.east()))
	{
		t.vertexUV(xf + 1.0f - 0.2f, (yf + 1.4f) + 0.0625f, zf + 0.0f, texU_2, texV_1);
		t.vertexUV(xf + 1.0f + 0.0f, (yf + 0.0f) + 0.0625f, zf + 0.0f, texU_2, texV_2);
		t.vertexUV(xf + 1.0f + 0.0f, (yf + 0.0f) + 0.0625f, zf + 1.0f, texU_1, texV_2);
		t.vertexUV(xf + 1.0f - 0.2f, (yf + 1.4f) + 0.0625f, zf + 1.0f, texU_1, texV_1);
		t.vertexUV(xf + 1.0f - 0.2f, (yf + 1.4f) + 0.0625f, zf + 1.0f, texU_1, texV_1);
		t.vertexUV(xf + 1.0f + 0.0f, (yf + 0.0f) + 0.0625f, zf + 1.0f, texU_1, texV_2);
		t.vertexUV(xf + 1.0f + 0.0f, (yf + 0.0f) + 0.0625f, zf + 0.0f, texU_2, texV_2);
		t.vertexUV(xf + 1.0f - 0.2f, (yf + 1.4f) + 0.0625f, zf + 0.0f, texU_2, texV_1);
	}

	if (pFireTile->canBurn(*m_pTileSource, pos.north()))
	{
		t.vertexUV(xf + 0.0f, (yf + 1.4f) + 0.0625f, zf + 0.2f, texU_1, texV_1);
		t.vertexUV(xf + 0.0f, (yf + 0.0f) + 0.0625f, zf + 0.0f, texU_1, texV_2);
		t.vertexUV(xf + 1.0f, (yf + 0.0f) + 0.0625f, zf + 0.0f, texU_2, texV_2);
		t.vertexUV(xf + 1.0f, (yf + 1.4f) + 0.0625f, zf + 0.2f, texU_2, texV_1);
		t.vertexUV(xf + 1.0f, (yf + 1.4f) + 0.0625f, zf + 0.2f, texU_2, texV_1);
		t.vertexUV(xf + 1.0f, (yf + 0.0f) + 0.0625f, zf + 0.0f, texU_2, texV_2);
		t.vertexUV(xf + 0.0f, (yf + 0.0f) + 0.0625f, zf + 0.0f, texU_1, texV_2);
		t.vertexUV(xf + 0.0f, (yf + 1.4f) + 0.0625f, zf + 0.2f, texU_1, texV_1);
	}

	if (pFireTile->canBurn(*m_pTileSource, pos.south()))
	{
		t.vertexUV(xf + 1.0f, (yf + 1.4f) + 0.0625f, zf + 1.0f - 0.2f, texU_2, texV_1);
		t.vertexUV(xf + 1.0f, (yf + 0.0f) + 0.0625f, zf + 1.0f + 0.0f, texU_2, texV_2);
		t.vertexUV(xf + 0.0f, (yf + 0.0f) + 0.0625f, zf + 1.0f + 0.0f, texU_1, texV_2);
		t.vertexUV(xf + 0.0f, (yf + 1.4f) + 0.0625f, zf + 1.0f - 0.2f, texU_1, texV_1);
		t.vertexUV(xf + 0.0f, (yf + 1.4f) + 0.0625f, zf + 1.0f - 0.2f, texU_1, texV_1);
		t.vertexUV(xf + 0.0f, (yf + 0.0f) + 0.0625f, zf + 1.0f + 0.0f, texU_1, texV_2);
		t.vertexUV(xf + 1.0f, (yf + 0.0f) + 0.0625f, zf + 1.0f + 0.0f, texU_2, texV_2);
		t.vertexUV(xf + 1.0f, (yf + 1.4f) + 0.0625f, zf + 1.0f - 0.2f, texU_2, texV_1);
	}

	if (pFireTile->canBurn(*m_pTileSource, pos.above()))
	{
		// @NOTE: Converting z and x to uint8_t for whatever reason
		if (((uint8_t(pos.z) + uint8_t(pos.x) + pos.y + 1) & 1) != 0)
		{
			float y1f = float(yf + 1);
			t.vertexUV(xf + 0.0f, y1f - 0.2f, zf + 0.5f + 0.5f, texU_2, texV_1);
			t.vertexUV(xf + 0.0f, y1f + 0.0f, zf + 0.5f - 0.5f, texU_2, texV_2);
			t.vertexUV(xf + 1.0f, y1f + 0.0f, zf + 0.5f - 0.5f, texU_1, texV_2);
			t.vertexUV(xf + 1.0f, y1f - 0.2f, zf + 0.5f + 0.5f, texU_1, texV_1);
			t.vertexUV(xf + 1.0f, y1f - 0.2f, zf + 0.5f - 0.5f, texU_2, texV_1);
			t.vertexUV(xf + 1.0f, y1f + 0.0f, zf + 0.5f + 0.5f, texU_2, texV_2);
			t.vertexUV(xf + 0.0f, y1f + 0.0f, zf + 0.5f + 0.5f, texU_1, texV_2);
			t.vertexUV(xf + 0.0f, y1f - 0.2f, zf + 0.5f - 0.5f, texU_1, texV_1);
		}
		else
		{
			float y1f = float(yf + 1);
			t.vertexUV(xf + 0.0f, y1f - 0.2f, zf + 0.0f, texU_2, texV_1);
			t.vertexUV(xf + 1.0f, y1f + 0.0f, zf + 0.0f, texU_2, texV_2);
			t.vertexUV(xf + 1.0f, y1f + 0.0f, zf + 1.0f, texU_1, texV_2);
			t.vertexUV(xf + 0.0f, y1f - 0.2f, zf + 1.0f, texU_1, texV_1);
			t.vertexUV(xf + 1.0f, y1f - 0.2f, zf + 1.0f, texU_2, texV_1);
			t.vertexUV(xf + 0.0f, y1f + 0.0f, zf + 1.0f, texU_2, texV_2);
			t.vertexUV(xf + 0.0f, y1f + 0.0f, zf + 0.0f, texU_1, texV_2);
			t.vertexUV(xf + 1.0f, y1f - 0.2f, zf + 0.0f, texU_1, texV_1);
		}
	}
	
	// @NOTE: Returning true when you may not necessarily have rendered something?
	return true;
}

bool TileRenderer::tesselateDustInWorld(Tile* tile, const TilePos& pos)
{
	Tesselator& t = Tesselator::instance; // var5
	int data = m_pTileSource->getData(pos); // var6
	int texture = tile->getTexture(Facing::UP, data); // var7
	if (m_fixedTexture >= 0)
	{
		texture = m_fixedTexture;
	}

	Color color = tile->getColor(*m_pTileSource, pos); // var8
	t.color(color);

	int xt = (texture & 15) << 4;
	int yt = texture & 240;
	float u0 = (float(xt) / 256.0f);
	float u1 = ((float(xt) + 15.99f) / 256.0f);
	float v0 = (float(yt) / 256.0f);
	float v1 = ((float(yt) + 15.99f) / 256.0f);
	float o = 0.0f;
	float r = 0.03125f;
	bool w = RedStoneDustTile::shouldConnectTo(*m_pTileSource, pos.west())
		|| (!m_pTileSource->isSolidBlockingTile(pos.west()) && RedStoneDustTile::shouldConnectTo(*m_pTileSource, pos.west().below()));
	bool e = RedStoneDustTile::shouldConnectTo(*m_pTileSource, pos.east())
		|| (!m_pTileSource->isSolidBlockingTile(pos.east()) && RedStoneDustTile::shouldConnectTo(*m_pTileSource, pos.east().below()));
	bool n = RedStoneDustTile::shouldConnectTo(*m_pTileSource, pos.north())
		|| (!m_pTileSource->isSolidBlockingTile(pos.north()) && RedStoneDustTile::shouldConnectTo(*m_pTileSource, pos.below().north()));
	bool s = RedStoneDustTile::shouldConnectTo(*m_pTileSource, pos.south())
		|| (!m_pTileSource->isSolidBlockingTile(pos.south()) && RedStoneDustTile::shouldConnectTo(*m_pTileSource, pos.below().south()));
	if (!m_pTileSource->isSolidBlockingTile(pos.above())) {
		if (m_pTileSource->isSolidBlockingTile(pos.west()) && RedStoneDustTile::shouldConnectTo(*m_pTileSource, pos.west().above())) {
			w = true;
		}

		if (m_pTileSource->isSolidBlockingTile(pos.east()) && RedStoneDustTile::shouldConnectTo(*m_pTileSource, pos.east().above())) {
			e = true;
		}

		if (m_pTileSource->isSolidBlockingTile(pos.north()) && RedStoneDustTile::shouldConnectTo(*m_pTileSource, pos.above().north())) {
			n = true;
		}

		if (m_pTileSource->isSolidBlockingTile(pos.south()) && RedStoneDustTile::shouldConnectTo(*m_pTileSource, pos.above().south())) {
			s = true;
		}
	}

	float d = 5.0f / 16.0f;
	float x0 = float(pos.x + 0);
	float x1 = float(pos.x + 1);
	float z0 = float(pos.z + 0);
	float z1 = float(pos.z + 1);
	int pic = 0;
	if ((w || e) && !n && !s) {
		pic = 1;
	}

	if ((n || s) && !e && !w) {
		pic = 2;
	}

	if (pic != 0) {
		u0 = (float(xt + 16) / 256.0f);
		u1 = ((float(xt + 16) + 15.99f) / 256.0f);
		v0 = (float(yt) / 256.0f);
		v1 = ((float(yt) + 15.99f) / 256.0f);
	}

	if (pic == 0) {
		if (e || n || s || w) {
			if (!w) {
				x0 += d;
			}

			if (!w) {
				u0 += (d / 16.0f);
			}

			if (!e) {
				x1 -= d;
			}

			if (!e) {
				u1 -= (d / 16.0f);
			}

			if (!n) {
				z0 += d;
			}

			if (!n) {
				v0 += (d / 16.0f);
			}

			if (!s) {
				z1 -= d;
			}

			if (!s) {
				v1 -= (d / 16.0f);
			}
		}

		t.vertexUV((x1 + o), (float(pos.y) + r), (z1 + o), u1, v1);
		t.vertexUV((x1 + o), (float(pos.y) + r), (z0 - o), u1, v0);
		t.vertexUV((x0 - o), (float(pos.y) + r), (z0 - o), u0, v0);
		t.vertexUV((x0 - o), (float(pos.y) + r), (z1 + o), u0, v1);
	}

	if (pic == 1) {
		t.vertexUV((x1 + o), (float(pos.y) + r), (z1 + o), u1, v1);
		t.vertexUV((x1 + o), (float(pos.y) + r), (z0 - o), u1, v0);
		t.vertexUV((x0 - o), (float(pos.y) + r), (z0 - o), u0, v0);
		t.vertexUV((x0 - o), (float(pos.y) + r), (z1 + o), u0, v1);
	}

	if (pic == 2) {
		t.vertexUV((x1 + o), (float(pos.y) + r), (z1 + o), u1, v1);
		t.vertexUV((x1 + o), (float(pos.y) + r), (z0 - o), u0, v1);
		t.vertexUV((x0 - o), (float(pos.y) + r), (z0 - o), u0, v0);
		t.vertexUV((x0 - o), (float(pos.y) + r), (z1 + o), u1, v0);
	}

	u0 = (float(xt + 16) / 256.0f);
	u1 = ((float(xt + 16) + 15.99f) / 256.0f);
	v0 = (float(yt) / 256.0f);
	v1 = ((float(yt) + 15.99f) / 256.0f);
	if (!m_pTileSource->isSolidBlockingTile(pos.above())) {
		if (m_pTileSource->isSolidBlockingTile(pos.west()) && m_pTileSource->getTile(pos.west().above()) == Tile::redStoneDust->m_ID) {
			t.vertexUV((float(pos.x) + r), (float(pos.y + 1) + o), (float(pos.z + 1) + o), u1, v0);
			t.vertexUV((float(pos.x) + r), (float(pos.y + 0) - o), (float(pos.z + 1) + o), u0, v0);
			t.vertexUV((float(pos.x) + r), (float(pos.y + 0) - o), (float(pos.z + 0) - o), u0, v1);
			t.vertexUV((float(pos.x) + r), (float(pos.y + 1) + o), (float(pos.z + 0) - o), u1, v1);
		}

		if (m_pTileSource->isSolidBlockingTile(pos.east()) && m_pTileSource->getTile(pos.east().above()) == Tile::redStoneDust->m_ID) {
			t.vertexUV((float(pos.x + 1) - r), (float(pos.y + 0) - o), (float(pos.z + 1) + o), u0, v1);
			t.vertexUV((float(pos.x + 1) - r), (float(pos.y + 1) + o), (float(pos.z + 1) + o), u1, v1);
			t.vertexUV((float(pos.x + 1) - r), (float(pos.y + 1) + o), (float(pos.z + 0) - o), u1, v0);
			t.vertexUV((float(pos.x + 1) - r), (float(pos.y + 0) - o), (float(pos.z + 0) - o), u0, v0);
		}

		if (m_pTileSource->isSolidBlockingTile(pos.north()) && m_pTileSource->getTile(pos.above().north()) == Tile::redStoneDust->m_ID) {
			t.vertexUV((float(pos.x + 1) + o), (float(pos.y + 0) - o), (float(pos.z) + r), u0, v1);
			t.vertexUV((float(pos.x + 1) + o), (float(pos.y + 1) + o), (float(pos.z) + r), u1, v1);
			t.vertexUV((float(pos.x + 0) - o), (float(pos.y + 1) + o), (float(pos.z) + r), u1, v0);
			t.vertexUV((float(pos.x + 0) - o), (float(pos.y + 0) - o), (float(pos.z) + r), u0, v0);
		}

		if (m_pTileSource->isSolidBlockingTile(pos.south()) && m_pTileSource->getTile(pos.above().south()) == Tile::redStoneDust->m_ID) {
			t.vertexUV((float(pos.x + 1) + o), (float(pos.y + 1) + o), (float(pos.z + 1) - r), u1, v0);
			t.vertexUV((float(pos.x + 1) + o), (float(pos.y + 0) - o), (float(pos.z + 1) - r), u0, v0);
			t.vertexUV((float(pos.x + 0) - o), (float(pos.y + 0) - o), (float(pos.z + 1) - r), u0, v1);
			t.vertexUV((float(pos.x + 0) - o), (float(pos.y + 1) + o), (float(pos.z + 1) - r), u1, v1);
		}
	}

	return true;
}

bool TileRenderer::tesselateInWorld(Tile* tile, const TilePos& pos)
{
	eRenderShape shape = tile->getRenderShape();
	tile->updateShape(*m_pTileSource, pos);

	//_tex1(getLightColor(tile, pos));

	switch (shape)
	{
		// @NOTE: In the original Minecraft PE, if a shape is undefined, the tile is invisible.
		// @NOTE: In v0.1.0, fire is invisible.
#ifndef ORIGINAL_CODE
		case SHAPE_NONE:
			return false;
		case SHAPE_FIRE:
			return tesselateFireInWorld(tile, pos);
		default:
#endif
		case SHAPE_SOLID:
			return tesselateBlockInWorld(tile, pos);
		case SHAPE_WATER:
			return tesselateWaterInWorld(tile, pos);
		case SHAPE_CROSS:
		case SHAPE_RANDOM_CROSS:
			return tesselateCrossInWorld(tile, pos);
		case SHAPE_CROPS:
			return tesselateRowInWorld(tile, pos);
		case SHAPE_TORCH:
			return tesselateTorchInWorld(tile, pos);
		case SHAPE_DUST:
			return tesselateDustInWorld(tile, pos);
		case SHAPE_LADDER:
			return tesselateLadderInWorld(tile, pos);
		case SHAPE_DOOR:
			return tesselateDoorInWorld(tile, pos);
		case SHAPE_STAIRS:
			return tesselateStairsInWorld(tile, pos);
		case SHAPE_FENCE:
			return tesselateFenceInWorld(tile, pos);
		case SHAPE_LEVER:
			return tesselateLeverInWorld(tile, pos);
		case SHAPE_DIODE:
			return tesselateDiodeInWorld(tile, pos);
		case SHAPE_FENCE_GATE:
			return tesselateFenceGateInWorld(tile, pos);
		case SHAPE_CACTUS:
			return tesselateBlockInWorld(tile, pos);
	}

	return false;
}

bool TileRenderer::tesselateInWorldNoCulling(Tile* tile, const TilePos& pos)
{
	m_bNoCulling = true;
	bool r = tesselateInWorld(tile, pos);
	m_bNoCulling = false;
	return r;
}

bool TileRenderer::tesselateInWorld(Tile* tile, const TilePos& pos, int a)
{
	m_fixedTexture = a;
	bool r = tesselateInWorld(tile, pos);
	m_fixedTexture = -1;
	return r;
}

Color getTileFaceColor(const FullTile& tile, Facing::Name face, float shading = 1.0f, bool preshade = false)
{
	Color color = tile.getType()->getColor(face, tile.data);
	if (preshade) color.mulRGB(shading);
	return color;
}

void TileRenderer::renderTile(const FullTile& tile, const mce::MaterialPtr& material, float bright, bool preshade)
{
	renderTile(tile, material, Color(bright, bright, bright), preshade);
}

#define SET_FACE_COLOR(face) t.color(color * getTileFaceColor(tile, face, _faceShades[m_lightingPreset][face], preshade && _shouldPreshadeFace[face]))
#define SET_FACE_NORMAL(face) t.normal(*_faceNormals[face])
#define _RENDER_FACE(face, func) \
	SET_FACE_COLOR(face); \
	SET_FACE_NORMAL(face); \
	func(tileType, Vec3::ZERO, tileType->getTexture(face, tile.data))

#define RENDER_DOWN()  _RENDER_FACE(Facing::DOWN,  renderFaceDown)
#define RENDER_UP()    _RENDER_FACE(Facing::UP,    renderFaceUp)
#define RENDER_NORTH() _RENDER_FACE(Facing::NORTH, renderNorth)
#define RENDER_SOUTH() _RENDER_FACE(Facing::SOUTH, renderSouth)
#define RENDER_WEST()  _RENDER_FACE(Facing::WEST,  renderWest)
#define RENDER_EAST()  _RENDER_FACE(Facing::EAST,  renderEast)

void TileRenderer::renderTile(const FullTile& tile, const mce::MaterialPtr& material, const Color& color, bool preshade)
{
	Tesselator& t = m_tessellator;
	Tile* tileType = tile.getType();
	eRenderShape shape = tileType->getRenderShape();
	tileType->updateDefaultShape();

	switch (shape)
	{
		case SHAPE_SOLID:
		default:
		{
			tileType->updateDefaultShape(); // otherwise pressure plates and buttons in the inventory would look all wonky
			t.addOffset(-0.5f, -0.5f, -0.5f);
			t.begin(4 * 6); // 4 to 24

			RENDER_UP();

			// N.B. If caller passes 255, they only want the face-down face.
			// This is a hack to accomodate the start menu screen procedurally generated title logo.
			if (tile.data != 255)
			{
				RENDER_DOWN();
				RENDER_NORTH();
				RENDER_SOUTH();
				RENDER_WEST();
				RENDER_EAST();
			}

			t.draw(material);
			t.addOffset(0.5f, 0.5f, 0.5f);
			break;
		}
		case SHAPE_CROSS:
		{
			// normally unused as cross items render like regular items in the hand
			// however, this is used for rendering the TripodCamera's stand
			t.begin(16);
			SET_FACE_COLOR(Facing::UP);
			SET_FACE_NORMAL(Facing::UP);
			tesselateCrossTexture(tile, Vec3(-0.5f, -0.5f, -0.5f), true);
			t.draw(material);
			break;
		}
		case SHAPE_STAIRS:
		{
			// Fixed version from 0.1.1j+
			t.addOffset(-0.5f, -0.5f, -0.5f);

			t.begin((4 * 6) * 2);
			for (int i = 0; i < 2; i++)
			{
				if (i == 0)
					tileType->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f);
				else
					tileType->setShape(0.0f, 0.0f, 0.5f, 1.0f, 0.5f, 1.0f);

				RENDER_DOWN();
				RENDER_UP();
				RENDER_NORTH();
				RENDER_SOUTH();
				RENDER_WEST();
				RENDER_EAST();
			}
			t.draw(material);

			t.addOffset(0.5f, 0.5f, 0.5f);
			break;
		}
		case SHAPE_FENCE:
		{
			t.addOffset(-0.5f, -0.5f, -0.5f);
			constexpr float v5 = 1.0f / 16.0f;
			constexpr float v6 = v5 * 2.0f;

			t.begin((4 * 6) * 4);
			for (int i = 0; i < 4; i++)
			{
				switch (i)
				{
				case 0: tileType->setShape(0.5f - v6, 0.0f,             0.0f,               0.5f + v6, 1.0f,      v6 * 2.0f       ); break;
				case 1: tileType->setShape(0.5f - v6, 0.0f,             1.0f - (v6 * 2.0f), 0.5f + v6, 1.0f,      1.0f            ); break;
				case 2: tileType->setShape(0.5f - v5, 1.0f - v5 * 3.0f, -v5 * 2.0f,         0.5f + v5, 1.0f - v5, 1.0f + v5 * 2.0f); break;
				case 3: tileType->setShape(0.5f - v5, 0.5f - v5 * 3.0f, -v5 * 2.0f,         0.5f + v5, 0.5f - v5, 1.0f + v5 * 2.0f); break;
				}

				RENDER_UP();
				RENDER_DOWN();
				RENDER_NORTH();
				RENDER_SOUTH();
				RENDER_WEST();
				RENDER_EAST();
			}
			t.draw(material);

			t.addOffset(0.5f, 0.5f, 0.5f);
			tileType->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
			break;
		}
		case SHAPE_FENCE_GATE:
			t.addOffset(-0.5f, -0.5f, -0.5f);

			t.begin((4 * 6) * 3);
			for (int o = 0; o < 3; o++)
			{
				constexpr float r = 0.0625f;
				switch (o)
				{
				case 0: tileType->setShape(0.5f - r, 0.3f, 0.0f,            0.5f + r, 1.0f,     r * 2.0f); break;
				case 1: tileType->setShape(0.5f - r, 0.3f, 1.0f - r * 2.0f, 0.5f + r, 1.0f,     1.0f    ); break;
				case 2: tileType->setShape(0.5f - r, 0.5f, 0.0f,            0.5f + r, 1.0f - r, 1.0f    ); break;
				}

				RENDER_UP();
				RENDER_DOWN();
				RENDER_NORTH();
				RENDER_SOUTH();
				RENDER_WEST();
				RENDER_EAST();
			}
			t.draw(material);

			t.addOffset(0.5f, 0.5f, 0.5f);
			tileType->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
			break;
		}
}

bool TileRenderer::tesselateBlockInWorldWithAmbienceOcclusionV2(Tile* tile, const TilePos& pos, float r, float g, float b)
{
	// START OF AUXILIARY DATA FOR AO

	// Well, this is a big enum.
	enum eDirection
	{
		ETILE_FACE_HERE,  // Here (no change in coordinates)
		ETILE_FACE_N,  // -Z
		ETILE_FACE_S,  // +Z
		ETILE_FACE_W,  // -X
		ETILE_FACE_E,  // +X
		ETILE_FACE_NW, // -X-Z
		ETILE_FACE_NE, // +X-Z
		ETILE_FACE_SW, // -X+Z
		ETILE_FACE_SE, // +X+Z

		ETILE_FACE_U,  // +Y
		ETILE_FACE_UN, // +Y-Z
		ETILE_FACE_US, // +Y+Z
		ETILE_FACE_UW, // +Y-X
		ETILE_FACE_UE, // +Y+X
		ETILE_FACE_UNW,// +Y-X-Z
		ETILE_FACE_UNE,// +Y+X-Z
		ETILE_FACE_USW,// +Y-X+Z
		ETILE_FACE_USE,// +Y+X+Z

		ETILE_FACE_D,  // -Y
		ETILE_FACE_DN, // -Y-Z
		ETILE_FACE_DS, // -Y+Z
		ETILE_FACE_DW, // -Y-X
		ETILE_FACE_DE, // -Y+X
		ETILE_FACE_DNW,// -Y-X-Z
		ETILE_FACE_DNE,// -Y+X-Z
		ETILE_FACE_DSW,// -Y-X+Z
		ETILE_FACE_DSE,// -Y+X+Z
		ETILE_FACE_COUNT
	};

	// for Facing::Name directions
	static const int diffX[] = { 0,0,0,0,-1,1 };
	static const int diffZ[] = { 0,0,-1,1,0,0 };
	static const int diffY[] = { -1,1,0,0,0,0 };

	// for ETILE_FACE_? directions
	static const int diffEX[] = { 0,0,0,-1,+1,-1,+1,-1,+1,0,0,0,-1,+1,-1,+1,-1,+1,0,0,0,-1,+1,-1,+1,-1,+1 };
	static const int diffEZ[] = { 0,-1,+1,0,0,-1,-1,+1,+1,0,-1,+1,0,0,-1,-1,+1,+1,0,-1,+1,0,0,-1,-1,+1,+1 };
	static const int diffEY[] = { 0,0,0,0,0,0,0,0,0, +1,+1,+1,+1,+1,+1,+1,+1,+1, -1,-1,-1,-1,-1,-1,-1,-1,-1 };

	// Convert a Facing::Name to an ETILE_FACE_?
	static const int dirToEdir[] = { ETILE_FACE_D, ETILE_FACE_U, ETILE_FACE_N, ETILE_FACE_S, ETILE_FACE_W, ETILE_FACE_E };

	// this is a huge table. Essentially this tells us which 3 tiles we should base our lighting value on aside from the
	// one that's pointed to by the face we're rendering.
	// The array is 6 * 4 * 3. 6 for the directions, 4 for the number of vertices per face, and 3 for the number of other tiles we'll check.

	static const int massLUT[] = {
		ETILE_FACE_DS, ETILE_FACE_DW, ETILE_FACE_DSW,   ETILE_FACE_DN, ETILE_FACE_DW, ETILE_FACE_DNW,   ETILE_FACE_DN, ETILE_FACE_DE, ETILE_FACE_DNE,   ETILE_FACE_DS, ETILE_FACE_DE, ETILE_FACE_DSE,
		ETILE_FACE_US, ETILE_FACE_UE, ETILE_FACE_USE,   ETILE_FACE_UN, ETILE_FACE_UE, ETILE_FACE_UNE,   ETILE_FACE_UN, ETILE_FACE_UW, ETILE_FACE_UNW,   ETILE_FACE_US, ETILE_FACE_UW, ETILE_FACE_USW,
		ETILE_FACE_UN, ETILE_FACE_NW, ETILE_FACE_UNW,   ETILE_FACE_UN, ETILE_FACE_NE, ETILE_FACE_UNE,   ETILE_FACE_DN, ETILE_FACE_NE, ETILE_FACE_DNE,   ETILE_FACE_DN, ETILE_FACE_NW, ETILE_FACE_DNW,
		ETILE_FACE_US, ETILE_FACE_SW, ETILE_FACE_USW,   ETILE_FACE_DS, ETILE_FACE_SW, ETILE_FACE_DSW,   ETILE_FACE_DS, ETILE_FACE_SE, ETILE_FACE_DSE,   ETILE_FACE_US, ETILE_FACE_SE, ETILE_FACE_USE,
		ETILE_FACE_UW, ETILE_FACE_SW, ETILE_FACE_USW,   ETILE_FACE_UW, ETILE_FACE_NW, ETILE_FACE_UNW,   ETILE_FACE_DW, ETILE_FACE_NW, ETILE_FACE_DNW,   ETILE_FACE_DW, ETILE_FACE_SW, ETILE_FACE_DSW,
		ETILE_FACE_DE, ETILE_FACE_SE, ETILE_FACE_DSE,   ETILE_FACE_DE, ETILE_FACE_NE, ETILE_FACE_DNE,   ETILE_FACE_UE, ETILE_FACE_NE, ETILE_FACE_UNE,   ETILE_FACE_UE, ETILE_FACE_SE, ETILE_FACE_USE,
	};

	// Depending on the face, add a multiplier to our lighting value. This is the same as in regular block rendering without AO.
	static const float lightingMult[] = { 0.5f, 1.0f, 0.8f, 0.8f, 0.6f, 0.6f };

	// END OF AUXILIARY DATA FOR AO

	bool bBother = false;

	// Check if we should bother at all. Most of the time, we don't. There are some calculations that we shouldn't do unless necessary
	for (int dir = Facing::DOWN; dir <= Facing::EAST; dir++)
	{
		TilePos tp(pos.x + diffX[dir],
				   pos.y + diffY[dir],
				   pos.z + diffZ[dir]);

		if (!m_bNoCulling && !tile->shouldRenderFace(*m_pTileSource, tp, (Facing::Name)dir))
			continue;

		bBother = true;
		break;
	}

	if (!bBother)
		return false;

	float topR = r, topG = g, topB = b;

	if (tile == Tile::grass)
		r = g = b = 1.0f;

	//Tesselator& t = m_tessellator;

	//float fLightHere = tile->getBrightness(*m_pTileSource, pos);

	float lights[ETILE_FACE_COUNT];

	// Get the brightness of the tile we're processing, as well as all tiles around it
	lights[ETILE_FACE_HERE] = tile->getBrightness(*m_pTileSource, pos);

	for (int i = 1; i < ETILE_FACE_COUNT; i++)
	{
		lights[i] = tile->getBrightness(*m_pTileSource, TilePos(pos.x + diffEX[i], pos.y + diffEY[i], pos.z + diffEZ[i]));
	}

	// Render all the faces.
	for (int dir = Facing::DOWN; dir <= Facing::EAST; dir++)
	{
		TilePos tp(pos.x + diffX[dir],
			pos.y + diffY[dir],
			pos.z + diffZ[dir]);

		// check if we should bother in the first place
		if (!m_bNoCulling && !tile->shouldRenderFace(*m_pTileSource, tp, (Facing::Name)dir))
		{
			continue;
		}

		float fR = r, fG = g, fB = b;
		if (dir == Facing::UP)
			fR = topR, fG = topG, fB = topB;

		for (int i = 0; i < 4; i++)
			m_vtxRed[i] = m_vtxGreen[i] = m_vtxBlue[i] = 1.0f;

		for (int i = 0; i < 4; i++)
			m_vtxLightTex[i] = 1.0f;

		const int* table = &massLUT[dir * 12];

		for (int i = 0; i < 4; i++)
		{
			// average: the light at the tile the face's normal would point towards, and 3 other tiles
			// chosen based on the vertex corner number
			float br = lights[dirToEdir[dir]];
			for (int j = 0; j < 3; j++)
				br += lights[table[j + i * 3]];

			br *= 0.25f;
				
			m_vtxRed[i]   = br;
			m_vtxGreen[i] = br;
			m_vtxBlue[i]  = br;
			m_vtxLightTex[i] = br;
		}

		for (int i = 0; i < 4; i++)
		{
			m_vtxRed  [i] *= fR * lightingMult[dir];
			m_vtxGreen[i] *= fG * lightingMult[dir];
			m_vtxBlue [i] *= fB * lightingMult[dir];
		}

		m_ambientOcclusion = true;

		switch (dir) 
		{
			case Facing::DOWN:
				renderFaceDown(tile, pos, tile->getTexture(*m_pTileSource, pos, Facing::DOWN));
				break;
			case Facing::UP:
				renderFaceUp  (tile, pos, tile->getTexture(*m_pTileSource, pos, Facing::UP));
				break;
			case Facing::NORTH:
				renderNorth   (tile, pos, tile->getTexture(*m_pTileSource, pos, Facing::NORTH));
				break;
			case Facing::SOUTH:
				renderSouth   (tile, pos, tile->getTexture(*m_pTileSource, pos, Facing::SOUTH));
				break;
			case Facing::WEST:
				renderWest    (tile, pos, tile->getTexture(*m_pTileSource, pos, Facing::WEST));
				break;
			case Facing::EAST:
				renderEast    (tile, pos, tile->getTexture(*m_pTileSource, pos, Facing::EAST));
				break;
		}

		if (m_fixedTexture == -1
			&& ((GrassTile*)tile)->m_bFancyGrass
			&& tile->getTexture(*m_pTileSource, pos, (Facing::Name)dir) == TEXTURE_GRASS_SIDE
			&& (dir == Facing::WEST || dir == Facing::EAST || dir == Facing::NORTH || dir == Facing::SOUTH))
		{
			for (int i = 0; i < 4; i++)
				m_vtxRed[i] = m_vtxGreen[i] = m_vtxBlue[i] = 1.0f;

			for (int i = 0; i < 4; i++)
			{
				// average: the light at the tile the face's normal would point towards, and 3 other tiles
				// chosen based on the vertex corner number
				float br = lights[dirToEdir[dir]];
				for (int j = 0; j < 3; j++)
					br += lights[table[j + i * 3]];

				br *= 0.25f;

				m_vtxRed[i]   = br;
				m_vtxGreen[i] = br;
				m_vtxBlue[i]  = br;
			}

			for (int i = 0; i < 4; i++)
			{
				m_vtxRed[i]   *= topR * lightingMult[dir];
				m_vtxGreen[i] *= topG * lightingMult[dir];
				m_vtxBlue[i]  *= topB * lightingMult[dir];
			}

			switch (dir) 
			{
				case Facing::DOWN:
					renderFaceDown(tile, pos, TEXTURE_GRASS_SIDE_OVERLAY);
					break;
				case Facing::UP:
					renderFaceUp(tile, pos, TEXTURE_GRASS_SIDE_OVERLAY);
					break;
				case Facing::NORTH:
					renderNorth(tile, pos, TEXTURE_GRASS_SIDE_OVERLAY);
					break;
				case Facing::SOUTH:
					renderSouth(tile, pos, TEXTURE_GRASS_SIDE_OVERLAY);
					break;
				case Facing::WEST:
					renderWest(tile, pos, TEXTURE_GRASS_SIDE_OVERLAY);
					break;
				case Facing::EAST:
					renderEast(tile, pos, TEXTURE_GRASS_SIDE_OVERLAY);
					break;
			}
		}

		m_ambientOcclusion = false;
	}

	return true;
}

bool TileRenderer::useAmbientOcclusion() const
{
	return Minecraft::useAmbientOcclusion;
}
