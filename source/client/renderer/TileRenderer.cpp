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
#include "world/tile/FireTile.hpp"
#include "world/tile/LiquidTile.hpp"
#include "GameMods.hpp"

bool TileRenderer::m_bFancyGrass = false;
bool TileRenderer::m_bBiomeColors = false;

void TileRenderer::_init()
{
	m_textureOverride = -1;
	field_8 = false;
	m_bDisableCulling = false;
	m_bAmbientOcclusion = false;
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

TileRenderer::TileRenderer()
{
	_init();
#ifndef ORIGINAL_CODE
	// @BUG: Not initializing level source
	m_pLevelSource = nullptr;
#endif
}

TileRenderer::TileRenderer(LevelSource* pLevelSource)
{
	_init();
	m_pLevelSource = pLevelSource;
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

		if (m_pLevelSource->getMaterial(TilePos(checkPos.x, checkPos.y + 1, checkPos.z)) == pCheckMtl)
			return 1.0f;

		Material* pMtl = m_pLevelSource->getMaterial(checkPos);
		if (pMtl == pCheckMtl)
		{
			TileData data = m_pLevelSource->getData(checkPos);
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
	return renderShape == SHAPE_SOLID || renderShape == SHAPE_STAIRS || renderShape == SHAPE_FENCE || renderShape == SHAPE_CACTUS;
}

// @NOTE: This sucks! Very badly! But it's how they did it.
void TileRenderer::renderEast(Tile* tile, const Vec3& pos, int texture)
{
	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->setShape(0.0625, 0, 0, 0.9375, 1, 1);

	static constexpr float C_RATIO = 1.0f / 256.0f;

	if (m_textureOverride >= 0)
		texture = m_textureOverride;

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
	else if (field_8)
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
		texV_u = C_RATIO * (texY + aabb.min.y * 16.0f);
		texV_d = C_RATIO * (texY + aabb.max.y * 16.0f - 0.01f);
	}

	Tesselator& t = Tesselator::instance;

	if (m_bAmbientOcclusion)
	{
		t.color(m_vtxRed[0], m_vtxGreen[0], m_vtxBlue[0]);
		t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_l, texV_d);
		t.color(m_vtxRed[1], m_vtxGreen[1], m_vtxBlue[1]);
		t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_r, texV_d);
		t.color(m_vtxRed[2], m_vtxGreen[2], m_vtxBlue[2]);
		t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_r, texV_u);
		t.color(m_vtxRed[3], m_vtxGreen[3], m_vtxBlue[3]);
		t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_l, texV_u);
		return;
	}
	
	t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_l, texV_d);
	t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_r, texV_d);
	t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_r, texV_u);
	t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_l, texV_u);

	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->updateShape(m_pLevelSource, pos);
}

void TileRenderer::renderWest(Tile* tile, const Vec3& pos, int texture)
{
	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->setShape(0.0625, 0, 0, 0.9375, 1, 1);

	static constexpr float C_RATIO = 1.0f / 256.0f;

	if (m_textureOverride >= 0)
		texture = m_textureOverride;

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
	else if (field_8)
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
		texV_u = C_RATIO * (texY + aabb.min.y * 16.0f);
		texV_d = C_RATIO * (texY + aabb.max.y * 16.0f - 0.01f);
	}

	Tesselator& t = Tesselator::instance;

	if (m_bAmbientOcclusion)
	{
		t.color(m_vtxRed[0], m_vtxGreen[0], m_vtxBlue[0]);
		t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_r, texV_u);
		t.color(m_vtxRed[1], m_vtxGreen[1], m_vtxBlue[1]);
		t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_l, texV_u);
		t.color(m_vtxRed[2], m_vtxGreen[2], m_vtxBlue[2]);
		t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_l, texV_d);
		t.color(m_vtxRed[3], m_vtxGreen[3], m_vtxBlue[3]);
		t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_r, texV_d);
		return;
	}
	
	t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_r, texV_u);
	t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_l, texV_u);
	t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_l, texV_d);
	t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_r, texV_d);

	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->updateShape(m_pLevelSource, pos);
}

void TileRenderer::renderSouth(Tile* tile, const Vec3& pos, int texture)
{
	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->setShape(0, 0, 0.0625, 1, 1, 0.9375);

	static constexpr float C_RATIO = 1.0f / 256.0f;

	if (m_textureOverride >= 0)
		texture = m_textureOverride;

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
	else if (field_8)
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
		texV_u = C_RATIO * (texY + aabb.min.y * 16.0f);
		texV_d = C_RATIO * (texY + aabb.max.y * 16.0f - 0.01f);
	}

	Tesselator& t = Tesselator::instance;

	if (m_bAmbientOcclusion)
	{
		t.color(m_vtxRed[0], m_vtxGreen[0], m_vtxBlue[0]);
		t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_l, texV_u);
		t.color(m_vtxRed[1], m_vtxGreen[1], m_vtxBlue[1]);
		t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_l, texV_d);
		t.color(m_vtxRed[2], m_vtxGreen[2], m_vtxBlue[2]);
		t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_r, texV_d);
		t.color(m_vtxRed[3], m_vtxGreen[3], m_vtxBlue[3]);
		t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_r, texV_u);
		return;
	}

	t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_l, texV_u);
	t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_l, texV_d);
	t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_r, texV_d);
	t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_r, texV_u);

	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->updateShape(m_pLevelSource, pos);
}

void TileRenderer::renderNorth(Tile* tile, const Vec3& pos, int texture)
{
	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->setShape(0, 0, 0.0625, 1, 1, 0.9375);

	static constexpr float C_RATIO = 1.0f / 256.0f;

	if (m_textureOverride >= 0)
		texture = m_textureOverride;

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
	else if (field_8)
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
		texV_u = C_RATIO * (texY + aabb.min.y * 16.0f);
		texV_d = C_RATIO * (texY + aabb.max.y * 16.0f - 0.01f);
	}

	Tesselator& t = Tesselator::instance;

	if (m_bAmbientOcclusion)
	{
		t.color(m_vtxRed[0], m_vtxGreen[0], m_vtxBlue[0]);
		t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_r, texV_u);
		t.color(m_vtxRed[1], m_vtxGreen[1], m_vtxBlue[1]);
		t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_l, texV_u);
		t.color(m_vtxRed[2], m_vtxGreen[2], m_vtxBlue[2]);
		t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_l, texV_d);
		t.color(m_vtxRed[3], m_vtxGreen[3], m_vtxBlue[3]);
		t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_r, texV_d);
		return;
	}

	t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_r, texV_u);
	t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_l, texV_u);
	t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_l, texV_d);
	t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_r, texV_d);

	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->updateShape(m_pLevelSource, pos);
}

void TileRenderer::renderFaceDown(Tile* tile, const Vec3& pos, int texture)
{
	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->setShape(0, 0, 0, 1, 1, 1);

	static constexpr float C_RATIO = 1.0f / 256.0f;

	if (m_textureOverride >= 0)
		texture = m_textureOverride;

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

	Tesselator& t = Tesselator::instance;

	if (m_bAmbientOcclusion)
	{
		t.color(m_vtxRed[0], m_vtxGreen[0], m_vtxBlue[0]);
		t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_2, texV_2);
		t.color(m_vtxRed[1], m_vtxGreen[1], m_vtxBlue[1]);
		t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_2, texV_1);
		t.color(m_vtxRed[2], m_vtxGreen[2], m_vtxBlue[2]);
		t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_1, texV_1);
		t.color(m_vtxRed[3], m_vtxGreen[3], m_vtxBlue[3]);
		t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_1, texV_2);
		return;
	}

	t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_2, texV_2);
	t.vertexUV(aabb.max.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_2, texV_1);
	t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.min.z + pos.z, texU_1, texV_1);
	t.vertexUV(aabb.min.x + pos.x, aabb.max.y + pos.y, aabb.max.z + pos.z, texU_1, texV_2);

	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->updateShape(m_pLevelSource, pos);
}

void TileRenderer::renderFaceUp(Tile* tile, const Vec3& pos, int texture)
{
	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->setShape(0, 0, 0, 1, 1, 1);
	
	static constexpr float C_RATIO = 1.0f / 256.0f;

	if (m_textureOverride >= 0)
		texture = m_textureOverride;

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

	Tesselator& t = Tesselator::instance;

	if (m_bAmbientOcclusion)
	{
		t.color(m_vtxRed[0], m_vtxGreen[0], m_vtxBlue[0]);
		t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_1, texV_2);
		t.color(m_vtxRed[1], m_vtxGreen[1], m_vtxBlue[1]);
		t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_1, texV_1);
		t.color(m_vtxRed[2], m_vtxGreen[2], m_vtxBlue[2]);
		t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_2, texV_1);
		t.color(m_vtxRed[3], m_vtxGreen[3], m_vtxBlue[3]);
		t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_2, texV_2);
		return;
	}

	t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_1, texV_2);
	t.vertexUV(aabb.min.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_1, texV_1);
	t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.min.z + pos.z, texU_2, texV_1);
	t.vertexUV(aabb.max.x + pos.x, aabb.min.y + pos.y, aabb.max.z + pos.z, texU_2, texV_2);

	if (tile->getRenderShape() == SHAPE_CACTUS)
		tile->updateShape(m_pLevelSource, pos);
}

void TileRenderer::tesselateCrossTexture(Tile* tile, TileData data, const Vec3& pos)
{
	static constexpr float C_RATIO = 1.0f / 256.0f;

	int texture = m_textureOverride;
	if (texture < 0)
		texture = tile->getTexture(Facing::DOWN, data);

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	// calculate U and V coordinates
	float texU_l = texX * C_RATIO, texU_r = (texX + 15.99f) * C_RATIO;
	float texV_u = texY * C_RATIO, texV_d = (texY + 15.99f) * C_RATIO;

	float cenX = pos.x + 0.5f, cenZ = pos.z + 0.5f;
	float newY = pos.y;

	if (tile->getRenderShape() == SHAPE_RANDOM_CROSS)
	{
		int64_t var17 = int64_t(pos.x * 3129871) ^ (int64_t)pos.z * 116129781L ^ (int64_t)pos.y;
		var17 = var17 * var17 * 42317861L + var17 * 11L;
		cenX += (((var17 >> 16 & 15L) / 15.0f) - 0.5f) * 0.5f;
		newY += (((var17 >> 20 & 15L) / 15.0f) - 1.0f) * 0.2f;
		cenZ += (((var17 >> 24 & 15L) / 15.0f) - 0.5f) * 0.5f;
	}
	
	float x1 = cenX - 0.45f, x2 = cenX + 0.45f;
	float z1 = cenZ - 0.45f, z2 = cenZ + 0.45f;

	Tesselator& t = Tesselator::instance;
	// face 1
	t.vertexUV(x1, newY + 1, z1, texU_l, texV_u);
	t.vertexUV(x1, newY + 0, z1, texU_l, texV_d);
	t.vertexUV(x2, newY + 0, z2, texU_r, texV_d);
	t.vertexUV(x2, newY + 1, z2, texU_r, texV_u);

	// face 2
	t.vertexUV(x2, newY + 1, z2, texU_l, texV_u);
	t.vertexUV(x2, newY + 0, z2, texU_l, texV_d);
	t.vertexUV(x1, newY + 0, z1, texU_r, texV_d);
	t.vertexUV(x1, newY + 1, z1, texU_r, texV_u);

	// face 3
	t.vertexUV(x1, newY + 1, z2, texU_l, texV_u);
	t.vertexUV(x1, newY + 0, z2, texU_l, texV_d);
	t.vertexUV(x2, newY + 0, z1, texU_r, texV_d);
	t.vertexUV(x2, newY + 1, z1, texU_r, texV_u);

	// face 4
	t.vertexUV(x2, newY + 1, z1, texU_l, texV_u);
	t.vertexUV(x2, newY + 0, z1, texU_l, texV_d);
	t.vertexUV(x1, newY + 0, z2, texU_r, texV_d);
	t.vertexUV(x1, newY + 1, z2, texU_r, texV_u);
}

bool TileRenderer::tesselateBlockInWorld(Tile* tile, const TilePos& pos, float r, float g, float b)
{
	float topR = r, topG = g, topB = b;

	if (tile == Tile::grass)
		r = g = b = 1.0f;

	Tesselator& t = Tesselator::instance;

	float fLightHere = tile->getBrightness(m_pLevelSource, pos);
	bool bDrewAnything = false;

	if (m_bDisableCulling || tile->shouldRenderFace(m_pLevelSource, pos.below(), Facing::DOWN))
	{
		bDrewAnything = true;

		float fLight = tile->getBrightness(m_pLevelSource, pos.below());
		t.color(r * 0.5f * fLight, g * 0.5f * fLight, b * 0.5f * fLight);

		renderFaceUp(tile, pos, tile->getTexture(m_pLevelSource, pos, Facing::DOWN));
	}

	if (m_bDisableCulling || tile->shouldRenderFace(m_pLevelSource, pos.above(), Facing::UP))
	{
		bDrewAnything = true;

		float fLight = tile->getBrightness(m_pLevelSource, pos.above());
		if (tile->m_aabb.max.y != 1.0f && !tile->m_pMaterial->isLiquid())
			fLight = fLightHere;

		t.color(topR * fLight, topG * fLight, topB * fLight);

		renderFaceDown(tile, pos, tile->getTexture(m_pLevelSource, pos, Facing::UP));
	}

	if (m_bDisableCulling || tile->shouldRenderFace(m_pLevelSource, pos.north(), Facing::NORTH))
	{
		bDrewAnything = true;

		float fLight = tile->getBrightness(m_pLevelSource, pos.north());
		if (tile->m_aabb.min.z > 0.0f)
			fLight = fLightHere;

		t.color(r * 0.8f * fLight, g * 0.8f * fLight, b * 0.8f * fLight);
		int texture = tile->getTexture(m_pLevelSource, pos, Facing::NORTH);
		renderNorth(tile, pos, texture);

		if (m_bFancyGrass && texture == TEXTURE_GRASS_SIDE && this->m_textureOverride < 0)
		{
			t.color(topR * 0.8f * fLight, topG * 0.8f * fLight, topB * 0.8f * fLight);
			renderNorth(tile, pos, TEXTURE_NONE84);
		}
	}

	if (m_bDisableCulling || tile->shouldRenderFace(m_pLevelSource, pos.south(), Facing::SOUTH))
	{
		bDrewAnything = true;

		float fLight = tile->getBrightness(m_pLevelSource, pos.south());
		if (tile->m_aabb.max.z < 1.0f)
			fLight = fLightHere;

		t.color(r * 0.8f * fLight, g * 0.8f * fLight, b * 0.8f * fLight);
		int texture = tile->getTexture(m_pLevelSource, pos, Facing::SOUTH);
		renderSouth(tile, pos, texture);

		if (m_bFancyGrass && texture == TEXTURE_GRASS_SIDE && this->m_textureOverride < 0)
		{
			t.color(topR * 0.8f * fLight, topG * 0.8f * fLight, topB * 0.8f * fLight);
			renderSouth(tile, pos, TEXTURE_NONE84);
		}
	}

	if (m_bDisableCulling || tile->shouldRenderFace(m_pLevelSource, pos.west(), Facing::WEST))
	{
		bDrewAnything = true;

		float fLight = tile->getBrightness(m_pLevelSource, pos.west());
		if (tile->m_aabb.min.x > 0.0f)
			fLight = fLightHere;

		t.color(r * 0.6f * fLight, g * 0.6f * fLight, b * 0.6f * fLight);
		int texture = tile->getTexture(m_pLevelSource, pos, Facing::WEST);
		renderWest(tile, pos, texture);

		if (m_bFancyGrass && texture == TEXTURE_GRASS_SIDE && this->m_textureOverride < 0)
		{
			t.color(topR * 0.6f * fLight, topG * 0.6f * fLight, topB * 0.6f * fLight);
			renderWest(tile, pos, TEXTURE_NONE84);
		}
	}

	if (m_bDisableCulling || tile->shouldRenderFace(m_pLevelSource, pos.east(), Facing::EAST))
	{
		bDrewAnything = true;

		float fLight = tile->getBrightness(m_pLevelSource, pos.east());
		if (tile->m_aabb.max.x < 1.0f)
			fLight = fLightHere;

		t.color(r * 0.6f * fLight, g * 0.6f * fLight, b * 0.6f * fLight);
		int texture = tile->getTexture(m_pLevelSource, pos, Facing::EAST);
		renderEast(tile, pos, texture);

		if (m_bFancyGrass && texture == TEXTURE_GRASS_SIDE && this->m_textureOverride < 0)
		{
			t.color(topR * 0.6f * fLight, topG * 0.6f * fLight, topB * 0.6f * fLight);
			renderEast(tile, pos, TEXTURE_NONE84);
		}
	}

	return bDrewAnything;
}

bool TileRenderer::tesselateBlockInWorld(Tile* tile, const TilePos& pos)
{
	int color = getTileColor(tile, pos);

	float r = float(GET_RED  (color)) / 255.0f;
	float g = float(GET_GREEN(color)) / 255.0f;
	float b = float(GET_BLUE (color)) / 255.0f;

	if (useAmbientOcclusion())
	{
#ifdef ENH_USE_OWN_AO
		return tesselateBlockInWorldWithAmbienceOcclusionV2(tile, pos, r, g, b);
#else
		return tesselateBlockInWorldWithAmbienceOcclusion(tile, pos, r, g, b);
#endif
	}

	return tesselateBlockInWorld(tile, pos, r, g, b);
}

bool TileRenderer::tesselateCrossInWorld(Tile* tile, const TilePos& pos)
{
	Tesselator& t = Tesselator::instance;

	float bright = tile->getBrightness(m_pLevelSource, pos);
	int color = getTileColor(tile, pos);
	float r = bright * (float(GET_RED(color)) / 255.0f);
	float g = bright * (float(GET_GREEN(color)) / 255.0f);
	float b = bright * (float(GET_BLUE(color)) / 255.0f);

	t.color(r, g, b);

	tesselateCrossTexture(tile, m_pLevelSource->getData(pos), pos);

	return true;
}

bool TileRenderer::tesselateWaterInWorld(Tile* tile1, const TilePos& pos)
{
	constexpr float C_RATIO = 1.0f / 256.0f;

	LiquidTile* tile = (LiquidTile*)tile1;
	bool bRenderFaceDown, bRenderFaceUp, bRenderSides[4];

	Tesselator& t = Tesselator::instance;

	bRenderFaceDown = tile->shouldRenderFace(m_pLevelSource, pos.above(), Facing::UP);
	bRenderFaceUp   = tile->shouldRenderFace(m_pLevelSource, pos.below(), Facing::DOWN);

	bRenderSides[0] = tile->shouldRenderFace(m_pLevelSource, pos.north(), Facing::NORTH);
	bRenderSides[1] = tile->shouldRenderFace(m_pLevelSource, pos.south(), Facing::SOUTH);
	bRenderSides[2] = tile->shouldRenderFace(m_pLevelSource, pos.west(), Facing::WEST);
	bRenderSides[3] = tile->shouldRenderFace(m_pLevelSource, pos.east(), Facing::EAST);

	if (!bRenderFaceDown &&
		!bRenderFaceUp   &&
		!bRenderSides[0] &&
		!bRenderSides[1] &&
		!bRenderSides[2] &&
		!bRenderSides[3])
		return false;

	int tileData = m_pLevelSource->getData(pos);
	float
		fHeight1 = getWaterHeight(pos, tile->m_pMaterial),
		fHeight2 = getWaterHeight(TilePos(pos.x, pos.y, pos.z + 1), tile->m_pMaterial),
		fHeight3 = getWaterHeight(TilePos(pos.x + 1, pos.y, pos.z + 1), tile->m_pMaterial),
		fHeight4 = getWaterHeight(pos.east(), tile->m_pMaterial);

	// @TODO: fix gotos
	bool bFlag1, bFlag2;
	if (!m_bDisableCulling)
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
		float slopeAngle = tile->getSlopeAngle(m_pLevelSource, pos, tile->m_pMaterial);

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

		float bright = tile->getBrightness(m_pLevelSource, pos);

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

	if (m_bDisableCulling)
		goto label_7;

	bFlag1 = true;
	if (bRenderFaceUp)
	{
	label_6:
	label_7:
		float bright = tile->getBrightness(m_pLevelSource, pos.below());
		t.color(bright * 0.5f, bright * 0.5f, bright * 0.5f);
		renderFaceUp(tile1, pos, tile->getTexture(Facing::DOWN));
		bFlag1 = true;
	}

label_8:
	bFlag2 = bFlag1;
	bool bRenderedSides = false;

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
		if (!m_bDisableCulling && !bRenderSides[dir])
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
		bRenderedSides = true;

		float brightMul = dir >= Facing::WEST ? 0.6f : 0.8f;
		float bright = tile->getBrightness(m_pLevelSource, TilePos(checkX, pos.y, checkZ));
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

	switch (m_pLevelSource->getData(pos))
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


	bool tileWest = m_pLevelSource->getTile(pos.west()) == tile->m_ID;
	bool tileEast = m_pLevelSource->getTile(pos.east()) == tile->m_ID;
	bool tileNorth = m_pLevelSource->getTile(pos.north()) == tile->m_ID;
	bool tileSouth = m_pLevelSource->getTile(pos.south()) == tile->m_ID;

	bool connectsHorizontally = tileWest || tileEast;
	bool connectsVertically = tileNorth || tileSouth;

	if (!connectsHorizontally && !connectsVertically) {
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

bool TileRenderer::tesselateDoorInWorld(Tile* tile, const TilePos& pos)
{
	Tesselator& t = Tesselator::instance;
	float fBrightHere = tile->getBrightness(m_pLevelSource, pos), fBright;
	int texture;

	fBright = tile->getBrightness(m_pLevelSource, pos.below());
	if (tile->m_aabb.min.y > 0.0f)       fBright = fBrightHere;
	if (Tile::lightEmission[tile->m_ID]) fBright = 1.0f;
	t.color(fBright * 0.5f, fBright * 0.5f, fBright * 0.5f);
	renderFaceUp(tile, pos, tile->getTexture(m_pLevelSource, pos, Facing::DOWN));

	fBright = tile->getBrightness(m_pLevelSource, pos.above());
	if (tile->m_aabb.max.y < 1.0f)       fBright = fBrightHere;
	if (Tile::lightEmission[tile->m_ID]) fBright = 1.0f;
	t.color(fBright, fBright, fBright);
	renderFaceDown(tile, pos, tile->getTexture(m_pLevelSource, pos, Facing::UP));

	fBright = tile->getBrightness(m_pLevelSource, pos - 1);
	if (tile->m_aabb.min.z > 0.0f)       fBright = fBrightHere;
	if (Tile::lightEmission[tile->m_ID]) fBright = 1.0f;
	t.color(fBright * 0.8f, fBright * 0.8f, fBright * 0.8f);
	texture = tile->getTexture(m_pLevelSource, pos, Facing::NORTH);
	if (texture < 0) texture = -texture, field_8 = true;
	renderNorth(tile, pos, texture);
	field_8 = false;

	fBright = tile->getBrightness(m_pLevelSource, pos + 1);
	if (tile->m_aabb.max.z < 1.0f)       fBright = fBrightHere;
	if (Tile::lightEmission[tile->m_ID]) fBright = 1.0f;
	t.color(fBright * 0.8f, fBright * 0.8f, fBright * 0.8f);
	texture = tile->getTexture(m_pLevelSource, pos, Facing::SOUTH);
	if (texture < 0) texture = -texture, field_8 = true;
	renderSouth(tile, pos, texture);
	field_8 = false;

	fBright = tile->getBrightness(m_pLevelSource,pos.west());
	if (tile->m_aabb.min.x > 0.0f)       fBright = fBrightHere;
	if (Tile::lightEmission[tile->m_ID]) fBright = 1.0f;
	t.color(fBright * 0.6f, fBright * 0.6f, fBright * 0.6f);
	texture = tile->getTexture(m_pLevelSource, pos, Facing::WEST);
	if (texture < 0) texture = -texture, field_8 = true;
	renderWest(tile, pos, texture);
	field_8 = false;

	fBright = tile->getBrightness(m_pLevelSource, pos.east());
	if (tile->m_aabb.max.x < 1.0f)       fBright = fBrightHere;
	if (Tile::lightEmission[tile->m_ID]) fBright = 1.0f;
	t.color(fBright * 0.6f, fBright * 0.6f, fBright * 0.6f);
	texture = tile->getTexture(m_pLevelSource, pos, Facing::EAST);
	if (texture < 0) texture = -texture, field_8 = true;
	renderEast(tile, pos, texture);
	field_8 = false;

	return true;
}

void TileRenderer::tesselateTorch(Tile* tile, const Vec3& pos, float a, float b)
{
	constexpr float C_RATIO = 1.0f / 256.0f;
	constexpr float C_ONE_PIXEL = 1.0f / 16.0f;
	constexpr float C_HALF_TILE = 1.0f / 2.0f;
	constexpr float C_TOP_SKEW_RATIO = 0.375f;

	int texture = tile->getTexture(Facing::DOWN);

	if (m_textureOverride >= 0)
		texture = m_textureOverride;

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

	Tesselator& t = Tesselator::instance;
	
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
	TileData data = m_pLevelSource->getData(pos);
	float bright = tile->getBrightness(m_pLevelSource, pos);

	if (Tile::lightEmission[tile->m_ID] > 0)
		bright = 1.0f;

	Tesselator& t = Tesselator::instance;
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

bool TileRenderer::tesselateLadderInWorld(Tile* tile, const TilePos& pos)
{
	constexpr float C_RATIO = 1.0f / 256.0f;

	Tesselator& t = Tesselator::instance;

	int texture = tile->getTexture(Facing::DOWN);

	if (m_textureOverride >= 0)
		texture = m_textureOverride;

	float bright = m_pLevelSource->getBrightness(pos);
	t.color(bright, bright, bright);

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	float texU_1 = C_RATIO * texX;
	float texU_2 = C_RATIO * (texX + 15.99f);
	float texV_1 = C_RATIO * texY;
	float texV_2 = C_RATIO * (texY + 15.99f);

	switch (m_pLevelSource->getData(pos))
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

	Tesselator& t = Tesselator::instance;

	int texture = tile->getTexture(Facing::DOWN);
	float bright = tile->getBrightness(m_pLevelSource, pos);

	t.color(bright, bright, bright);

	FireTile* pFireTile = (FireTile*)Tile::fire;

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	float texU_1 = C_RATIO * (texX);
	float texU_2 = C_RATIO * (texX + 15.99f);
	float texV_1 = C_RATIO * (texY);
	float texV_2 = C_RATIO * (texY + 15.99f);
	float xf = float(pos.x), yf = float(pos.y), zf = float(pos.z);

	if (m_pLevelSource->isSolidTile(pos.below()) || pFireTile->canBurn(m_pLevelSource, pos.below()))
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

	if (pFireTile->canBurn(m_pLevelSource, pos.west()))
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

	if (pFireTile->canBurn(m_pLevelSource, pos.east()))
	{
		t.vertexUV(xf + 1.0f - 0.2f, (pos.y + 1.4f) + 0.0625f, pos.z + 0.0f, texU_2, texV_1);
		t.vertexUV(xf + 1.0f + 0.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 0.0f, texU_2, texV_2);
		t.vertexUV(xf + 1.0f + 0.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 1.0f, texU_1, texV_2);
		t.vertexUV(xf + 1.0f - 0.2f, (pos.y + 1.4f) + 0.0625f, pos.z + 1.0f, texU_1, texV_1);
		t.vertexUV(xf + 1.0f - 0.2f, (pos.y + 1.4f) + 0.0625f, pos.z + 1.0f, texU_1, texV_1);
		t.vertexUV(xf + 1.0f + 0.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 1.0f, texU_1, texV_2);
		t.vertexUV(xf + 1.0f + 0.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 0.0f, texU_2, texV_2);
		t.vertexUV(xf + 1.0f - 0.2f, (pos.y + 1.4f) + 0.0625f, pos.z + 0.0f, texU_2, texV_1);
	}

	if (pFireTile->canBurn(m_pLevelSource, pos - 1))
	{
		t.vertexUV(pos.x + 0.0f, (pos.y + 1.4f) + 0.0625f, pos.z + 0.2f, texU_1, texV_1);
		t.vertexUV(pos.x + 0.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 0.0f, texU_1, texV_2);
		t.vertexUV(pos.x + 1.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 0.0f, texU_2, texV_2);
		t.vertexUV(pos.x + 1.0f, (pos.y + 1.4f) + 0.0625f, pos.z + 0.2f, texU_2, texV_1);
		t.vertexUV(pos.x + 1.0f, (pos.y + 1.4f) + 0.0625f, pos.z + 0.2f, texU_2, texV_1);
		t.vertexUV(pos.x + 1.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 0.0f, texU_2, texV_2);
		t.vertexUV(pos.x + 0.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 0.0f, texU_1, texV_2);
		t.vertexUV(pos.x + 0.0f, (pos.y + 1.4f) + 0.0625f, pos.z + 0.2f, texU_1, texV_1);
	}

	if (pFireTile->canBurn(m_pLevelSource, pos + 1))
	{
		t.vertexUV(pos.x + 1.0f, (pos.y + 1.4f) + 0.0625f, pos.z + 1.0f - 0.2f, texU_2, texV_1);
		t.vertexUV(pos.x + 1.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 1.0f + 0.0f, texU_2, texV_2);
		t.vertexUV(pos.x + 0.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 1.0f + 0.0f, texU_1, texV_2);
		t.vertexUV(pos.x + 0.0f, (pos.y + 1.4f) + 0.0625f, pos.z + 1.0f - 0.2f, texU_1, texV_1);
		t.vertexUV(pos.x + 0.0f, (pos.y + 1.4f) + 0.0625f, pos.z + 1.0f - 0.2f, texU_1, texV_1);
		t.vertexUV(pos.x + 0.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 1.0f + 0.0f, texU_1, texV_2);
		t.vertexUV(pos.x + 1.0f, (pos.y + 0.0f) + 0.0625f, pos.z + 1.0f + 0.0f, texU_2, texV_2);
		t.vertexUV(pos.x + 1.0f, (pos.y + 1.4f) + 0.0625f, pos.z + 1.0f - 0.2f, texU_2, texV_1);
	}

	if (pFireTile->canBurn(m_pLevelSource, pos.above()))
	{
		// @NOTE: Converting z and x to uint8_t for whatever reason
		if (((uint8_t(pos.z) + uint8_t(pos.x) + pos.y + 1) & 1) != 0)
		{
			float y1f = float(pos.y + 1);
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
			float y1f = float(pos.y + 1);
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

bool TileRenderer::tesselateInWorld(Tile* tile, const TilePos& pos)
{
	int shape = tile->getRenderShape();
	tile->updateShape(m_pLevelSource, pos);

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
		case SHAPE_TORCH:
			return tesselateTorchInWorld(tile, pos);
		case SHAPE_LADDER:
			return tesselateLadderInWorld(tile, pos);
		case SHAPE_DOOR:
			return tesselateDoorInWorld(tile, pos);
		case SHAPE_STAIRS:
			return tesselateStairsInWorld(tile, pos);
		case SHAPE_FENCE:
			return tesselateFenceInWorld(tile, pos);
		case SHAPE_CACTUS:
			return tesselateBlockInWorld(tile, pos);
	}

	return false;
}

bool TileRenderer::tesselateInWorldNoCulling(Tile* tile, const TilePos& pos)
{
	m_bDisableCulling = true;
	bool r = tesselateInWorld(tile, pos);
	m_bDisableCulling = false;
	return r;
}

bool TileRenderer::tesselateInWorld(Tile* tile, const TilePos& pos, int a)
{
	m_textureOverride = a;
	bool r = tesselateInWorld(tile, pos);
	m_textureOverride = -1;
	return r;
}

bool TileRenderer::tesselateBlockInWorldWithAmbienceOcclusion(Tile* a2, const TilePos& pos, float r, float g, float b)
{
	float v12; // r0
	LevelSource* v13; // r1
	float v14; // r0
	LevelSource* v15; // r1
	float v16; // r0
	LevelSource* v17; // r1
	float v18; // r0
	LevelSource* v19; // r1
	float v20; // r0
	LevelSource* v21; // r1
	float v22; // r0
	LevelSource* v23; // r1
	char v24; // r3
	LevelSource* v25; // r0
	char v26; // r3
	LevelSource* v27; // r0
	char v28; // r3
	LevelSource* v29; // r0
	char v30; // r3
	LevelSource* v31; // r0
	char v32; // r3
	LevelSource* v33; // r0
	char v34; // r3
	LevelSource* v35; // r0
	char v36; // r3
	LevelSource* v37; // r0
	char v38; // r3
	LevelSource* v39; // r0
	char v40; // r3
	LevelSource* v41; // r0
	char v42; // r3
	LevelSource* v43; // r0
	char v44; // r3
	LevelSource* v45; // r0
	int v46; // r8
	float v47; // r0
	LevelSource* v48; // r1
	float v49; // r0
	LevelSource* v50; // r1
	float v51; // r0
	LevelSource* v52; // r1
	float v53; // r0
	bool v54; // r3
	float v55; // s12
	float v56; // s11
	float v57; // s8
	float v58; // s14
	float v59; // s13
	float v60; // s9
	float v61; // s10
	float v62; // s13
	float v63; // s11
	float v64; // s10
	float v65; // s14
	float v66; // s15
	float v67; // s12
	LevelSource* v68; // r1
	int v69; // r8
	int v70; // r0
	float v71; // r0
	LevelSource* v72; // r1
	float v73; // r0
	LevelSource* v74; // r1
	float v75; // r0
	LevelSource* v76; // r1
	float v77; // r0
	bool v78; // r3
	float v79; // s13
	float v80; // s10
	float v81; // s9
	float v82; // s11
	float v83; // s12
	float v84; // s15
	float v85; // s12
	float v86; // s10
	float v87; // s15
	float v88; // s13
	float v89; // s14
	LevelSource* v90; // r1
	int v91; // r0
	float v92; // r0
	LevelSource* v93; // r1
	float v94; // r0
	LevelSource* v95; // r1
	float v96; // r0
	LevelSource* v97; // r1
	float v98; // r0
	bool v99; // r3
	float v100; // s13
	float v101; // s11
	float v102; // s8
	float v103; // s14
	float v104; // s12
	float v105; // s10
	float v106; // s11
	float v107; // s12
	float v108; // s9
	float v109; // s10
	float v110; // s11
	float v111; // s13
	float v112; // s15
	float v113; // s14
	float v114; // s12
	LevelSource* v115; // r1
	int v116; // r0
	float v117; // r0
	LevelSource* v118; // r1
	float v119; // r0
	LevelSource* v120; // r1
	float v121; // r0
	LevelSource* v122; // r1
	float v123; // r0
	bool v124; // r3
	float v125; // s15
	float v126; // s11
	float v127; // s8
	float v128; // s12
	float v129; // s13
	float v130; // s9
	float v131; // s10
	float v132; // s13
	float v133; // s11
	float v134; // s10
	float v135; // s15
	float v136; // s14
	float v137; // s12
	LevelSource* v138; // r1
	int v139; // r0
	float v140; // r0
	LevelSource* v141; // r1
	float v142; // r0
	LevelSource* v143; // r1
	float v144; // r0
	LevelSource* v145; // r1
	float v146; // r0
	bool v147; // r3
	float v148; // s12
	float v149; // s9
	float v150; // s11
	float v151; // s14
	float v152; // s10
	float v153; // s13
	float v154; // s9
	float v155; // s12
	float v156; // s13
	float v157; // s9
	float v158; // s10
	float v159; // s11
	float v160; // s15
	float v161; // s14
	float v162; // s12
	LevelSource* v163; // r1
	int v164; // r0
	float v165; // r0
	LevelSource* v166; // r1
	float v167; // r0
	LevelSource* v168; // r1
	float v169; // r0
	LevelSource* v170; // r1
	float v171; // r0
	bool v172; // r3
	float v173; // r0
	bool v174; // r3
	float v175; // r0
	bool v176; // r3
	float v177; // r0
	float v178; // s15
	float v179; // s13
	float v180; // s10
	float v181; // s12
	float v182; // s9
	float v183; // s11
	float v184; // s13
	float v185; // s15
	float v186; // s13
	float v187; // s11
	float v188; // s10
	float v189; // s14
	float v190; // s15
	float v191; // s12
	LevelSource* v192; // r1
	int v193; // r0
	int result; // r0
	bool v195; // r3
	bool v196; // r3
	float v197; // r0
	bool v198; // r3
	float v199; // r0
	bool v200; // r3
	float v201; // r0
	bool v202; // r3
	float v203; // r0
	float v204; // r0
	bool v205; // r3
	float v206; // r0
	float v207; // r0
	float v208; // r0
	bool v209; // r3
	float v210; // r0
	float v211; // r0
	bool v212; // r3
	float v213; // r0
	int v214; // [sp+Ch] [bp-5Ch]
	int v215; // [sp+10h] [bp-58h]
	int v216; // [sp+14h] [bp-54h]
	int v217; // [sp+18h] [bp-50h]
	int v218; // [sp+1Ch] [bp-4Ch]
	int v219; // [sp+20h] [bp-48h]
	int v220; // [sp+24h] [bp-44h]
	int v221; // [sp+28h] [bp-40h]
	int v222; // [sp+2Ch] [bp-3Ch]

	this->m_bAmbientOcclusion = true;
	v12 = a2->getBrightness(this->m_pLevelSource, pos);
	v13 = this->m_pLevelSource;
	this->field_C = v12;
	v14 = a2->getBrightness(v13, pos.west());
	v218 = pos.y - 1;
	v15 = this->m_pLevelSource;
	this->field_10 = v14;
	v16 = a2->getBrightness(v15, pos.below());
	v217 = pos.z - 1;
	v17 = this->m_pLevelSource;
	this->field_14 = v16;
	v18 = a2->getBrightness(v17, pos.north());
	v214 = pos.x + 1;
	v19 = this->m_pLevelSource;
	this->field_18 = v18;
	v20 = a2->getBrightness(v19, pos.east());
	v216 = pos.y + 1;
	v21 = this->m_pLevelSource;
	this->field_1C = v20;
	v22 = a2->getBrightness(v21, pos.above());
	v215 = pos.z + 1;
	v23 = this->m_pLevelSource;
	this->field_20 = v22;
	this->field_24 = a2->getBrightness(v23, pos.south());
	v24 = Tile::translucent[this->m_pLevelSource->getTile(pos.above().east())];
	v25 = this->m_pLevelSource;
	this->field_AD = v24;
	v26 = Tile::translucent[v25->getTile(pos.below().east())];
	v27 = this->m_pLevelSource;
	this->field_B5 = v26;
	v28 = Tile::translucent[v27->getTile(pos.south().east())];
	v29 = this->m_pLevelSource;
	this->field_B1 = v28;
	v30 = Tile::translucent[v29->getTile(pos.north().east())];
	v31 = this->m_pLevelSource;
	this->field_B3 = v30;
	v32 = Tile::translucent[v31->getTile(pos.above().west())];
	v33 = this->m_pLevelSource;
	this->field_AE = v32;
	v34 = Tile::translucent[v33->getTile(pos.below().west())];
	v35 = this->m_pLevelSource;
	this->field_B6 = v34;
	v36 = Tile::translucent[v35->getTile(TilePos(pos.west() - 1))];
	v37 = this->m_pLevelSource;
	this->field_B0 = v36;
	v38 = Tile::translucent[v37->getTile(TilePos(pos.west() + 1))];
	v39 = this->m_pLevelSource;
	this->field_B2 = v38;
	v40 = Tile::translucent[v39->getTile(pos.above().south())];
	v41 = this->m_pLevelSource;
	this->field_AF = v40;
	v42 = Tile::translucent[v41->getTile(pos.above().north())];
	v43 = this->m_pLevelSource;
	this->field_AC = v42;
	v44 = Tile::translucent[v43->getTile(pos.below().south())];
	v45 = this->m_pLevelSource;
	this->field_B7 = v44;
	this->field_B4 = Tile::translucent[v45->getTile(pos.below().north())];
	if (a2->m_TextureFrame == 3)
	{
		v219 = 0;
		v220 = 0;
		v46 = 0;
		v221 = 0;
		v222 = 0;
	}
	else
	{
		v46 = 1;
		v219 = 1;
		v220 = 1;
		v221 = 1;
		v222 = 1;
	}
	if (!this->m_bDisableCulling
		&& !a2->shouldRenderFace(this->m_pLevelSource, TilePos(pos.x, v218, pos.z), Facing::DOWN))
	{
		v69 = 0;
		goto LABEL_20;
	}
	if (this->field_78 > 0)
	{
		v47 = a2->getBrightness(this->m_pLevelSource, TilePos(pos.x - 1, v218, pos.z));
		v48 = this->m_pLevelSource;
		this->field_2C = v47;
		v49 = a2->getBrightness(v48, TilePos(pos.x, v218, v217));
		v50 = this->m_pLevelSource;
		this->field_34 = v49;
		v51 = a2->getBrightness(v50, TilePos(pos.x, v218, v215));
		v52 = this->m_pLevelSource;
		this->field_38 = v51;
		v53 = a2->getBrightness(v52, TilePos(v214, v218, pos.z));
		v54 = this->field_B4;
		this->field_40 = v53;
		if (v54 || this->field_B6)
			this->field_28 = a2->getBrightness(this->m_pLevelSource, TilePos(pos.x - 1, v218, v217));
		else
			this->field_28 = this->field_2C;
		if (this->field_B7 || this->field_B6)
			this->field_30 = a2->getBrightness(this->m_pLevelSource, TilePos(pos.x - 1, v218, v215));
		else
			this->field_30 = this->field_2C;
		if (this->field_B4 || this->field_B5)
			this->field_3C = a2->getBrightness(this->m_pLevelSource, TilePos(v214, v218, v217));
		else
			this->field_3C = this->field_40;
		if (this->field_B7 || this->field_B5)
		{
			v203 = a2->getBrightness(this->m_pLevelSource, TilePos(v214, v218, v215));
			v55 = this->field_40;
			v56 = v203;
			this->field_44 = v203;
		}
		else
		{
			v55 = this->field_40;
			v56 = v55;
			this->field_44 = v55;
		}
		v57 = this->field_2C;
		v58 = this->field_14;
		v59 = this->field_38;
		v60 = (float)((float)((float)(v57 + this->field_30) + v59) + v58) * 0.25f;
		v61 = this->field_34;
		v62 = (float)((float)(v56 + (float)(v59 + v58)) + v55) * 0.25f;
		v63 = (float)((float)(v55 + (float)(v58 + v61)) + this->field_3C) * 0.25f;
		v64 = (float)(v61 + (float)(v58 + (float)(v57 + this->field_28))) * 0.25f;
		if (v46)
			goto LABEL_18;
	LABEL_34:
		v66 = 0.5f;
		this->m_vtxRed[3] = 0.5f;
		this->m_vtxRed[2] = 0.5f;
		this->m_vtxRed[1] = 0.5f;
		this->m_vtxRed[0] = 0.5f;
		this->m_vtxGreen[3] = 0.5f;
		this->m_vtxGreen[2] = 0.5f;
		this->m_vtxGreen[1] = 0.5f;
		this->m_vtxGreen[0] = 0.5f;
		v65 = 0.5f;
		v67 = 0.5f;
		goto LABEL_19;
	}
	v62 = this->field_14;
	v63 = v62;
	v64 = v62;
	v60 = v62;
	if (!v46)
		goto LABEL_34;
LABEL_18:
	v65 = r * 0.5f;
	v66 = g * 0.5f;
	this->m_vtxRed[3] = r * 0.5f;
	this->m_vtxRed[2] = r * 0.5f;
	this->m_vtxRed[1] = r * 0.5f;
	this->m_vtxRed[0] = r * 0.5f;
	v67 = b * 0.5f;
	this->m_vtxGreen[3] = g * 0.5f;
	this->m_vtxGreen[2] = g * 0.5f;
	this->m_vtxGreen[1] = g * 0.5f;
	this->m_vtxGreen[0] = g * 0.5f;
LABEL_19:
	v68 = this->m_pLevelSource;
	v69 = 1;
	this->m_vtxRed[0] = v60 * v65;
	this->m_vtxGreen[0] = v60 * v66;
	this->m_vtxBlue[0] = v60 * v67;
	this->m_vtxRed[1] = v64 * v65;
	this->m_vtxGreen[1] = v64 * v66;
	this->m_vtxBlue[1] = v64 * v67;
	this->m_vtxRed[2] = v63 * v65;
	this->m_vtxGreen[2] = v63 * v66;
	this->m_vtxGreen[3] = v62 * v66;
	this->m_vtxBlue[2] = v63 * v67;
	this->m_vtxRed[3] = v62 * v65;
	this->m_vtxBlue[3] = v62 * v67;
	v70 = a2->getTexture(v68, pos, Facing::DOWN);
	renderFaceUp(a2, pos, v70);
LABEL_20:
	if (this->m_bDisableCulling || a2->shouldRenderFace(this->m_pLevelSource, TilePos(pos.x, v216, pos.z), Facing::UP))
	{
		if (this->field_78 <= 0)
		{
			v87 = this->field_20;
			v89 = v87;
			v88 = v87;
			v85 = v87;
			goto LABEL_36;
		}
		v71 = a2->getBrightness(this->m_pLevelSource, TilePos(pos.x - 1, v216, pos.z));
		v72 = this->m_pLevelSource;
		this->field_4C = v71;
		v73 = a2->getBrightness(v72, TilePos(v214, v216, pos.z));
		v74 = this->m_pLevelSource;
		this->field_5C = v73;
		v75 = a2->getBrightness(v74, TilePos(pos.x, v216, v217));
		v76 = this->m_pLevelSource;
		this->field_54 = v75;
		v77 = a2->getBrightness(v76, TilePos(pos.x, v216, v215));
		v78 = this->field_AC;
		this->field_60 = v77;
		if (v78 || this->field_AE)
		{
			v197 = a2->getBrightness(this->m_pLevelSource, TilePos(pos.x - 1, v216, v217));
			v198 = this->field_AC;
			this->field_48 = v197;
			if (v198)
				goto LABEL_125;
		}
		else
		{
			this->field_48 = this->field_4C;
		}
		if (!this->field_AD)
		{
			this->field_58 = this->field_5C;
		LABEL_27:
			if (this->field_AF || this->field_AE)
			{
				v211 = a2->getBrightness(this->m_pLevelSource, TilePos(pos.x - 1, v216, v215));
				v212 = this->field_AF;
				this->field_50 = v211;
				if (v212)
					goto LABEL_141;
			}
			else
			{
				this->field_50 = this->field_4C;
			}
			if (!this->field_AD)
			{
				v79 = this->field_5C;
				v80 = v79;
				this->field_64 = v79;
			LABEL_32:
				v81 = this->field_4C;
				v82 = this->field_20;
				v83 = this->field_60;
				v84 = (float)(v81 + this->field_50) + v83;
				v85 = (float)((float)(v80 + (float)(v83 + v82)) + v79) * 0.25f;
				v86 = this->field_54;
				v87 = (float)(v84 + v82) * 0.25f;
				v88 = (float)((float)(v79 + (float)(v82 + v86)) + this->field_58) * 0.25f;
				v89 = (float)(v86 + (float)(v82 + (float)(v81 + this->field_48))) * 0.25f;
			LABEL_36:
				v90 = this->m_pLevelSource;
				v69 = 1;
				this->m_vtxRed[0] = v85 * r;
				this->m_vtxGreen[0] = v85 * g;
				this->m_vtxBlue[0] = v85 * b;
				this->m_vtxRed[1] = v88 * r;
				this->m_vtxGreen[1] = v88 * g;
				this->m_vtxBlue[1] = v88 * b;
				this->m_vtxRed[2] = v89 * r;
				this->m_vtxGreen[2] = v89 * g;
				this->m_vtxBlue[2] = v89 * b;
				this->m_vtxRed[3] = v87 * r;
				this->m_vtxGreen[3] = v87 * g;
				this->m_vtxBlue[3] = v87 * b;
				v91 = a2->getTexture(v90, pos, Facing::UP);
				renderFaceDown(a2, pos, v91);
				goto LABEL_37;
			}
		LABEL_141:
			v213 = a2->getBrightness(this->m_pLevelSource, TilePos(v214, v216, v215));
			v79 = this->field_5C;
			v80 = v213;
			this->field_64 = v213;
			goto LABEL_32;
		}
	LABEL_125:
		this->field_58 = a2->getBrightness(this->m_pLevelSource, TilePos(v214, v216, v217));
		goto LABEL_27;
	}
LABEL_37:
	if (!this->m_bDisableCulling && !a2->shouldRenderFace(this->m_pLevelSource, TilePos(pos.x, pos.y, v217), Facing::NORTH))
	{
		if (this->m_bDisableCulling)
			goto LABEL_54;
		goto LABEL_107;
	}
	if (this->field_78 <= 0)
	{
		v111 = this->field_18;
		v110 = v111;
		v109 = v111;
		v108 = v111;
		if (!v222)
			goto LABEL_50;
		goto LABEL_52;
	}
	v92 = a2->getBrightness(this->m_pLevelSource, TilePos(pos.x - 1, pos.y, v217));
	v93 = this->m_pLevelSource;
	this->field_68 = v92;
	v94 = a2->getBrightness(v93, TilePos(pos.x, v218, v217));
	v95 = this->m_pLevelSource;
	this->field_34 = v94;
	v96 = a2->getBrightness(v95, TilePos(pos.x, v216, v217));
	v97 = this->m_pLevelSource;
	this->field_54 = v96;
	v98 = a2->getBrightness(v97, TilePos(v214, pos.y, v217));
	v99 = this->field_B0;
	this->field_6C = v98;
	if (v99 || this->field_B4)
	{
		v201 = a2->getBrightness(this->m_pLevelSource, TilePos(pos.x - 1, v218, v217));
		v202 = this->field_B0;
		this->field_28 = v201;
		if (v202)
			goto LABEL_131;
	}
	else
	{
		this->field_28 = this->field_68;
	}
	if (this->field_AC)
	{
	LABEL_131:
		this->field_48 = a2->getBrightness(this->m_pLevelSource, TilePos(pos.x - 1, v216, v217));
		goto LABEL_44;
	}
	this->field_48 = this->field_68;
LABEL_44:
	if (this->field_B3 || this->field_B4)
	{
		v204 = a2->getBrightness(this->m_pLevelSource, TilePos(v214, v218, v217));
		v205 = this->field_B3;
		this->field_3C = v204;
		if (v205)
			goto LABEL_136;
	}
	else
	{
		this->field_3C = this->field_6C;
	}
	if (!this->field_AC)
	{
		v100 = this->field_6C;
		v101 = v100;
		this->field_58 = v100;
		goto LABEL_49;
	}
LABEL_136:
	v206 = a2->getBrightness(this->m_pLevelSource, TilePos(v214, v216, v217));
	v100 = this->field_6C;
	v101 = v206;
	this->field_58 = v206;
LABEL_49:
	v102 = this->field_68;
	v103 = this->field_18;
	v104 = this->field_54;
	v105 = (float)((float)(v102 + this->field_48) + v103) + v104;
	v106 = v101 + (float)((float)(v103 + v104) + v100);
	v107 = this->field_34;
	v108 = v105 * 0.25f;
	v109 = v106 * 0.25f;
	v110 = (float)(v100 + (float)((float)(v103 + v107) + this->field_3C)) * 0.25f;
	v111 = (float)(v103 + (float)(v107 + (float)(v102 + this->field_28))) * 0.25f;
	if (!v222)
	{
	LABEL_50:
		v112 = 0.8f;
		this->m_vtxRed[3] = 0.8f;
		this->m_vtxRed[2] = 0.8f;
		this->m_vtxRed[1] = 0.8f;
		this->m_vtxRed[0] = 0.8f;
		this->m_vtxGreen[3] = 0.8f;
		this->m_vtxGreen[2] = 0.8f;
		this->m_vtxGreen[1] = 0.8f;
		this->m_vtxGreen[0] = 0.8f;
		v113 = 0.8f;
		v114 = 0.8f;
		goto LABEL_53;
	}
LABEL_52:
	v113 = r * 0.8f;
	v112 = g * 0.8f;
	this->m_vtxRed[3] = r * 0.8f;
	this->m_vtxRed[2] = r * 0.8f;
	this->m_vtxRed[1] = r * 0.8f;
	this->m_vtxRed[0] = r * 0.8f;
	v114 = b * 0.8f;
	this->m_vtxGreen[3] = g * 0.8f;
	this->m_vtxGreen[2] = g * 0.8f;
	this->m_vtxGreen[1] = g * 0.8f;
	this->m_vtxGreen[0] = g * 0.8f;
LABEL_53:
	v115 = this->m_pLevelSource;
	v69 = 1;
	this->m_vtxRed[0] = v108 * v113;
	this->m_vtxGreen[0] = v108 * v112;
	this->m_vtxBlue[0] = v108 * v114;
	this->m_vtxRed[1] = v109 * v113;
	this->m_vtxGreen[1] = v109 * v112;
	this->m_vtxBlue[1] = v109 * v114;
	this->m_vtxRed[2] = v110 * v113;
	this->m_vtxGreen[2] = v110 * v112;
	this->m_vtxGreen[3] = v111 * v112;
	this->m_vtxBlue[2] = v110 * v114;
	this->m_vtxRed[3] = v111 * v113;
	this->m_vtxBlue[3] = v111 * v114;
	v116 = a2->getTexture(v115, pos, Facing::NORTH);
	renderNorth(a2, pos, v116);
	if (this->m_bDisableCulling)
		goto LABEL_54;
LABEL_107:
	if (!a2->shouldRenderFace(this->m_pLevelSource, TilePos(pos.x, pos.y, v215), Facing::SOUTH))
	{
		if (this->m_bDisableCulling)
			goto LABEL_70;
		goto LABEL_109;
	}
LABEL_54:
	if (this->field_78 <= 0)
	{
		v132 = this->field_24;
		v133 = v132;
		v134 = v132;
		v130 = v132;
		if (!v221)
			goto LABEL_66;
		goto LABEL_68;
	}
	v117 = a2->getBrightness(this->m_pLevelSource, TilePos(pos.x - 1, pos.y, v215));
	v118 = this->m_pLevelSource;
	this->field_70 = v117;
	v119 = a2->getBrightness(v118, TilePos(v214, pos.y, v215));
	v120 = this->m_pLevelSource;
	this->field_74 = v119;
	v121 = a2->getBrightness(v120, TilePos(pos.x, v218, v215));
	v122 = this->m_pLevelSource;
	this->field_38 = v121;
	v123 = a2->getBrightness(v122, TilePos(pos.x, v216, v215));
	v124 = this->field_B2;
	this->field_60 = v123;
	if (v124 || this->field_B7)
	{
		v199 = a2->getBrightness(this->m_pLevelSource, TilePos(pos.x - 1, v218, v215));
		v200 = this->field_B2;
		this->field_30 = v199;
		if (v200)
			goto LABEL_129;
	}
	else
	{
		this->field_30 = this->field_70;
	}
	if (this->field_AF)
	{
	LABEL_129:
		this->field_50 = a2->getBrightness(this->m_pLevelSource, TilePos(pos.x - 1, v216, v215));
		goto LABEL_60;
	}
	this->field_50 = this->field_70;
LABEL_60:
	if (this->field_B1 || this->field_B7)
	{
		v208 = a2->getBrightness(this->m_pLevelSource, TilePos(v214, v218, v215));
		v209 = this->field_B1;
		this->field_44 = v208;
		if (v209)
			goto LABEL_139;
	}
	else
	{
		this->field_44 = this->field_74;
	}
	if (!this->field_AF)
	{
		v125 = this->field_74;
		v126 = v125;
		this->field_64 = v125;
		goto LABEL_65;
	}
LABEL_139:
	v210 = a2->getBrightness(this->m_pLevelSource, TilePos(v214, v216, v215));
	v125 = this->field_74;
	v126 = v210;
	this->field_64 = v210;
LABEL_65:
	v127 = this->field_70;
	v128 = this->field_24;
	v129 = this->field_60;
	v130 = (float)((float)((float)(v127 + this->field_50) + v128) + v129) * 0.25f;
	v131 = this->field_38;
	v132 = (float)(v126 + (float)((float)(v128 + v129) + v125)) * 0.25f;
	v133 = (float)(v125 + (float)((float)(v128 + v131) + this->field_44)) * 0.25f;
	v134 = (float)(v128 + (float)(v131 + (float)(v127 + this->field_30))) * 0.25f;
	if (!v221)
	{
	LABEL_66:
		v135 = 0.8f;
		this->m_vtxRed[3] = 0.8f;
		this->m_vtxRed[2] = 0.8f;
		this->m_vtxRed[1] = 0.8f;
		this->m_vtxRed[0] = 0.8f;
		this->m_vtxGreen[3] = 0.8f;
		this->m_vtxGreen[2] = 0.8f;
		this->m_vtxGreen[1] = 0.8f;
		this->m_vtxGreen[0] = 0.8f;
		v136 = 0.8f;
		v137 = 0.8f;
		goto LABEL_69;
	}
LABEL_68:
	v136 = r * 0.8f;
	v135 = g * 0.8f;
	this->m_vtxRed[3] = r * 0.8f;
	this->m_vtxRed[2] = r * 0.8f;
	this->m_vtxRed[1] = r * 0.8f;
	this->m_vtxRed[0] = r * 0.8f;
	v137 = b * 0.8f;
	this->m_vtxGreen[3] = g * 0.8f;
	this->m_vtxGreen[2] = g * 0.8f;
	this->m_vtxGreen[1] = g * 0.8f;
	this->m_vtxGreen[0] = g * 0.8f;
LABEL_69:
	v138 = this->m_pLevelSource;
	v69 = 1;
	this->m_vtxRed[0] = v130 * v136;
	this->m_vtxGreen[0] = v130 * v135;
	this->m_vtxBlue[0] = v130 * v137;
	this->m_vtxRed[1] = v134 * v136;
	this->m_vtxGreen[1] = v134 * v135;
	this->m_vtxBlue[1] = v134 * v137;
	this->m_vtxRed[2] = v133 * v136;
	this->m_vtxGreen[2] = v133 * v135;
	this->m_vtxGreen[3] = v132 * v135;
	this->m_vtxBlue[2] = v133 * v137;
	this->m_vtxRed[3] = v132 * v136;
	this->m_vtxBlue[3] = v132 * v137;
	v139 = a2->getTexture(v138, pos, Facing::SOUTH);
	renderSouth(a2, pos, v139);
	if (this->m_bDisableCulling)
		goto LABEL_70;
LABEL_109:
	if (!a2->shouldRenderFace(this->m_pLevelSource, pos.west(), Facing::WEST))
	{
		if (this->m_bDisableCulling)
			goto LABEL_88;
		goto LABEL_111;
	}
LABEL_70:
	if (this->field_78 <= 0)
	{
		v156 = this->field_10;
		v159 = v156;
		v158 = v156;
		v157 = v156;
		if (!v220)
		{
		LABEL_84:
			v160 = 0.6f;
			this->m_vtxRed[3] = 0.6f;
			this->m_vtxRed[2] = 0.6f;
			this->m_vtxRed[1] = 0.6f;
			this->m_vtxRed[0] = 0.6f;
			this->m_vtxGreen[3] = 0.6f;
			this->m_vtxGreen[2] = 0.6f;
			this->m_vtxGreen[1] = 0.6f;
			this->m_vtxGreen[0] = 0.6f;
			v161 = 0.6f;
			v162 = 0.6f;
			goto LABEL_87;
		}
	}
	else
	{
		v140 = a2->getBrightness(this->m_pLevelSource, TilePos(pos.x - 1, v218, pos.z));
		v141 = this->m_pLevelSource;
		this->field_2C = v140;
		v142 = a2->getBrightness(v141, TilePos(pos.x - 1, pos.y, v217));
		v143 = this->m_pLevelSource;
		this->field_68 = v142;
		v144 = a2->getBrightness(v143, TilePos(pos.x - 1, pos.y, v215));
		v145 = this->m_pLevelSource;
		this->field_70 = v144;
		v146 = a2->getBrightness(v145, TilePos(pos.x - 1, v216, pos.z));
		v147 = this->field_B0;
		this->field_4C = v146;
		if (v147 || this->field_B6)
			this->field_28 = a2->getBrightness(this->m_pLevelSource, TilePos(pos.x - 1, v218, v217));
		else
			this->field_28 = this->field_68;
		if (this->field_B2 || this->field_B6)
			this->field_30 = a2->getBrightness(this->m_pLevelSource, TilePos(pos.x - 1, v218, v215));
		else
			this->field_30 = this->field_70;
		if (this->field_B0 || this->field_AE)
			this->field_48 = a2->getBrightness(this->m_pLevelSource, TilePos(pos.x - 1, v216, v217));
		else
			this->field_48 = this->field_68;
		if (this->field_B2 || this->field_AE)
		{
			v207 = a2->getBrightness(this->m_pLevelSource, TilePos(pos.x - 1, v216, v215));
			v148 = this->field_70;
			v149 = v207;
			this->field_50 = v207;
		}
		else
		{
			v148 = this->field_70;
			v149 = v148;
			this->field_50 = v148;
		}
		v150 = this->field_2C;
		v151 = this->field_10;
		v152 = this->field_4C;
		v153 = (float)((float)(v150 + this->field_30) + v151) + v148;
		v154 = v149 + (float)((float)(v151 + v148) + v152);
		v155 = this->field_68;
		v156 = v153 * 0.25f;
		v157 = v154 * 0.25f;
		v158 = (float)(v152 + (float)((float)(v151 + v155) + this->field_48)) * 0.25f;
		v159 = (float)(v151 + (float)(v155 + (float)(v150 + this->field_28))) * 0.25f;
		if (!v220)
			goto LABEL_84;
	}
	v161 = r * 0.6f;
	v160 = g * 0.6f;
	this->m_vtxRed[3] = r * 0.6f;
	this->m_vtxRed[2] = r * 0.6f;
	this->m_vtxRed[1] = r * 0.6f;
	this->m_vtxRed[0] = r * 0.6f;
	v162 = b * 0.6f;
	this->m_vtxGreen[3] = g * 0.6f;
	this->m_vtxGreen[2] = g * 0.6f;
	this->m_vtxGreen[1] = g * 0.6f;
	this->m_vtxGreen[0] = g * 0.6f;
LABEL_87:
	v163 = this->m_pLevelSource;
	v69 = 1;
	this->m_vtxRed[0] = v161 * v157;
	this->m_vtxGreen[0] = v160 * v157;
	this->m_vtxBlue[0] = v162 * v157;
	this->m_vtxRed[1] = v161 * v158;
	this->m_vtxGreen[1] = v160 * v158;
	this->m_vtxBlue[1] = v162 * v158;
	this->m_vtxRed[2] = v161 * v159;
	this->m_vtxGreen[2] = v160 * v159;
	this->m_vtxGreen[3] = v160 * v156;
	this->m_vtxBlue[2] = v162 * v159;
	this->m_vtxRed[3] = v161 * v156;
	this->m_vtxBlue[3] = v162 * v156;
	v164 = a2->getTexture(v163, pos, Facing::WEST);
	renderWest(a2, pos, v164);
	if (this->m_bDisableCulling)
		goto LABEL_88;
LABEL_111:
	if (!a2->shouldRenderFace(this->m_pLevelSource, TilePos(v214, pos.y, pos.z), Facing::EAST))
		goto LABEL_102;
LABEL_88:
	if (this->field_78 <= 0)
	{
		v186 = this->field_1C;
		v187 = v186;
		v188 = v186;
		v182 = v186;
		if (v219)
			goto LABEL_100;
	LABEL_104:
		v190 = 0.6f;
		this->m_vtxRed[3] = 0.6f;
		this->m_vtxRed[2] = 0.6f;
		this->m_vtxRed[1] = 0.6f;
		this->m_vtxRed[0] = 0.6f;
		this->m_vtxGreen[3] = 0.6f;
		this->m_vtxGreen[2] = 0.6f;
		this->m_vtxGreen[1] = 0.6f;
		this->m_vtxGreen[0] = 0.6f;
		v189 = 0.6f;
		v191 = 0.6f;
		goto LABEL_101;
	}
	v165 = a2->getBrightness(this->m_pLevelSource, TilePos(v214, v218, pos.z));
	v166 = this->m_pLevelSource;
	this->field_40 = v165;
	v167 = a2->getBrightness(v166, TilePos(v214, pos.y, v217));
	v168 = this->m_pLevelSource;
	this->field_6C = v167;
	v169 = a2->getBrightness(v168, TilePos(v214, pos.y, v215));
	v170 = this->m_pLevelSource;
	this->field_74 = v169;
	v171 = a2->getBrightness(v170, TilePos(v214, v216, pos.z));
	v172 = this->field_B5;
	this->field_5C = v171;
	if (v172 || this->field_B3)
	{
		v173 = a2->getBrightness(this->m_pLevelSource, TilePos(v214, v218, v217));
		v174 = this->field_B5;
		this->field_3C = v173;
		if (v174 || this->field_B1)
			goto LABEL_93;
	LABEL_121:
		this->field_44 = this->field_74;
		goto LABEL_94;
	}
	v196 = this->field_B1;
	this->field_3C = this->field_6C;
	if (!v196)
		goto LABEL_121;
LABEL_93:
	this->field_44 = a2->getBrightness(this->m_pLevelSource, TilePos(v214, v218, v215));
LABEL_94:
	if (this->field_AD || this->field_B3)
	{
		v175 = a2->getBrightness(this->m_pLevelSource, TilePos(v214, v216, v217));
		v176 = this->field_AD;
		this->field_58 = v175;
		if (v176 || this->field_B1)
			goto LABEL_98;
	}
	else
	{
		v195 = this->field_B1;
		this->field_58 = this->field_6C;
		if (v195)
		{
		LABEL_98:
			v177 = a2->getBrightness(this->m_pLevelSource, TilePos(v214, v216, v215));
			v178 = this->field_74;
			v179 = v177;
			this->field_64 = v177;
			goto LABEL_99;
		}
	}
	v178 = this->field_74;
	v179 = v178;
	this->field_64 = v178;
LABEL_99:
	v180 = this->field_40;
	v181 = this->field_1C;
	v182 = (float)((float)((float)(v180 + this->field_44) + v181) + v178) * 0.25f;
	v183 = this->field_5C;
	v184 = v179 + (float)((float)(v181 + v178) + v183);
	v185 = this->field_6C;
	v186 = v184 * 0.25f;
	v187 = (float)(v183 + (float)((float)(v181 + v185) + this->field_58)) * 0.25f;
	v188 = (float)(v181 + (float)(v185 + (float)(v180 + this->field_3C))) * 0.25f;
	if (!v219)
		goto LABEL_104;
LABEL_100:
	v189 = r * 0.6f;
	v190 = g * 0.6f;
	this->m_vtxRed[3] = r * 0.6f;
	this->m_vtxRed[2] = r * 0.6f;
	this->m_vtxRed[1] = r * 0.6f;
	this->m_vtxRed[0] = r * 0.6f;
	v191 = b * 0.6f;
	this->m_vtxGreen[3] = g * 0.6f;
	this->m_vtxGreen[2] = g * 0.6f;
	this->m_vtxGreen[1] = g * 0.6f;
	this->m_vtxGreen[0] = g * 0.6f;
LABEL_101:
	v192 = this->m_pLevelSource;
	v69 = 1;
	this->m_vtxRed[0] = v189 * v182;
	this->m_vtxGreen[0] = v190 * v182;
	this->m_vtxBlue[0] = v191 * v182;
	this->m_vtxRed[1] = v189 * v188;
	this->m_vtxGreen[1] = v190 * v188;
	this->m_vtxBlue[1] = v191 * v188;
	this->m_vtxRed[2] = v189 * v187;
	this->m_vtxGreen[2] = v190 * v187;
	this->m_vtxGreen[3] = v190 * v186;
	this->m_vtxBlue[2] = v191 * v187;
	this->m_vtxRed[3] = v189 * v186;
	this->m_vtxBlue[3] = v191 * v186;
	v193 = a2->getTexture(v192, pos, Facing::EAST);
	renderEast(a2, pos, v193);
LABEL_102:
	result = v69;
	this->m_bAmbientOcclusion = false;
	return result;
}

// this is very hacky
#ifdef ENH_SHADE_HELD_TILES

#ifdef MOD_DONT_COLOR_GRASS
#	define SHADE_IS_DECOLOR_GRASS_DEFINED true
#else
#	define SHADE_IS_DECOLOR_GRASS_DEFINED false
#endif

#define SHADE_DEFINE float red = 1, grn = 1, blu = 1

#define SHADE_PREPARE do { \
	red = bright, grn = bright, blu = bright; \
	if (GetPatchManager()->IsGrassTinted()) {           \
		if (tile->m_ID == Tile::leaves->m_ID)           \
			red *= 0.35f, grn *= 0.65f, blu *= 0.25f;   \
		if (tile->m_ID == Tile::grass->m_ID) \
			red *= 0.25f, grn *= 0.60f, blu *= 0.25f;   \
	}                                                   \
} while (0)

#define SHADE_IF_NEEDED(col) if (preshade) t.color(col*red,col*grn,col*blu,1.0f); else t.color(red,grn,blu,1.0f)

#define SHADE_FIXUP_GRASS do {  \
	if (tile->m_ID == Tile::grass->m_ID)          \
		red = bright, grn = bright, blu = bright; \
} while (0)

#else

#define SHADE_DEFINE           0
#define SHADE_PREPARE          0
#define SHADE_IF_NEEDED(col)   0
#define SHADE_FIXUP_GRASS(col) 0
#define SHADE_FIXUP_GRASS      0

#endif

void TileRenderer::renderTile(Tile* tile, TileData data, float bright, bool preshade)
{
	Tesselator& t = Tesselator::instance;

#ifndef ENH_SHADE_HELD_TILES
	bright = 1.0f; // 255
#endif
#ifndef USE_GL_NORMAL_LIGHTING
	preshade = true;
#endif

	int shape = tile->getRenderShape();
	switch (shape)
	{
		case SHAPE_SOLID:
		default:
		{
			// N.B. If caller passes 255, they only want the face-down face.
			// This is a hack to accomodate the start menu screen procedurally generated title logo.
#define IF_NEEDED(x) do { if (data != 255) { (x); } } while (0)

			glTranslatef(-0.5f, -0.5f, -0.5f);
			t.begin();
			SHADE_DEFINE;
			SHADE_PREPARE;
			SHADE_IF_NEEDED(1.0f);
			// Despite how it looks, Facing::UP is necessary for this to function correctly
			// Why? no idea
			t.normal(0.0f, 1.0f, 0.0f);
			renderFaceDown(tile, Vec3::ZERO, tile->getTexture(Facing::UP, data));
			SHADE_FIXUP_GRASS;
			SHADE_IF_NEEDED(0.5f);
			// Despite how it looks, Facing::DOWN is necessary for this to function correctly
			// Why? no idea
			t.normal(0.0f, -1.0f, 0.0f);
			IF_NEEDED(renderFaceUp(tile, Vec3::ZERO, tile->getTexture(Facing::DOWN, data)));
			SHADE_IF_NEEDED(0.8f);
			t.normal(0.0f, 0.0f, -1.0f);
			IF_NEEDED(renderNorth(tile, Vec3::ZERO, tile->getTexture(Facing::NORTH, data)));
			t.normal(0.0f, 0.0f, 1.0f);
			IF_NEEDED(renderSouth(tile, Vec3::ZERO, tile->getTexture(Facing::SOUTH, data)));
			SHADE_IF_NEEDED(0.6f);
			t.normal(-1.0f, 0.0f, 0.0f);
			IF_NEEDED(renderWest (tile, Vec3::ZERO, tile->getTexture(Facing::WEST, data)));
			t.normal(1.0f, 0.0f, 0.0f);
			IF_NEEDED(renderEast (tile, Vec3::ZERO, tile->getTexture(Facing::EAST, data)));
			SHADE_IF_NEEDED(1.0f);
			t.draw();
			glTranslatef(0.5f, 0.5f, 0.5f);
			break;
		}
		case SHAPE_CROSS:
		{
			// unused as cross items render like regular items in the hand
			t.begin();
			t.normal(0.0f, -1.0f, 0.0f);
			tesselateCrossTexture(tile, data, Vec3(-0.5f, -0.5f, -0.5f));
			t.draw();
			break;
		}
		case SHAPE_STAIRS:
		{
			// Fixed version from 0.1.1j+
			t.addOffset(-0.5f, -0.5f, -0.5f);
			for (int i = 0; i < 2; i++)
			{
				if (!i)
					tile->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f);
				else
					tile->setShape(0.0f, 0.0f, 0.5f, 1.0f, 0.5f, 1.0f);


				t.begin();
				SHADE_DEFINE;
				SHADE_PREPARE;
				SHADE_IF_NEEDED(0.5f);
				t.normal(0.0f, -1.0f, 0.0f);
				renderFaceUp  (tile, Vec3::ZERO, tile->getTexture(Facing::DOWN, data));
				SHADE_IF_NEEDED(1.0f);
				t.normal(0.0f, 1.0f, 0.0f);
				renderFaceDown(tile, Vec3::ZERO, tile->getTexture(Facing::UP, data));
				SHADE_IF_NEEDED(0.6f);
				t.normal(0.0f, 0.0f, -1.0f);
				renderNorth   (tile, Vec3::ZERO, tile->getTexture(Facing::NORTH, data));
				t.normal(0.0f, 0.0f, 1.0f);
				renderSouth   (tile, Vec3::ZERO, tile->getTexture(Facing::SOUTH, data));
				SHADE_IF_NEEDED(0.8f);
				t.normal(-1.0f, 0.0f, 0.0f);
				renderWest    (tile, Vec3::ZERO, tile->getTexture(Facing::WEST, data));
				t.normal(1.0f, 0.0f, 0.0f);
				renderEast    (tile, Vec3::ZERO, tile->getTexture(Facing::EAST, data));
				SHADE_IF_NEEDED(1.0f);
				t.draw();
			}
			t.addOffset(0.5f, 0.5f, 0.5f);
			break;
		}
		case SHAPE_FENCE:
		{
			t.addOffset(-0.5f, -0.5f, -0.5f);
			float v5 = 1.0f / 16.0f;
			float v6 = v5 * 2.0f;
			for (int i = 0; i < 4; i++)
			{
				switch (i) {
				case 0: tile->setShape(0.5f - v6, 0.0f, 0.0f, 0.5f + v6, 1.0f, v6 * 2.0f); break;
				case 1: tile->setShape(0.5f - v6, 0.0f, 1.0f - (v6 * 2.0f), 0.5f + v6, 1.0f, 1.0f); break;
				case 2: tile->setShape(0.5f - v5, 1.0f - v5 * 3.0f, -v5 * 2.0f, 0.5f + v5, 1.0f - v5, 1.0f + v5 * 2.0f); break;
				case 3: tile->setShape(0.5f - v5, 0.5f - v5 * 3.0f, -v5 * 2.0f, 0.5f + v5, 0.5f - v5, 1.0f + v5 * 2.0f); break;
				}

				t.begin();
				SHADE_DEFINE;
				SHADE_PREPARE;
				SHADE_IF_NEEDED(1.0f);
				t.normal(0.0f, 1.0f, 0.0f);
				renderFaceDown(tile, Vec3::ZERO, tile->getTexture(Facing::UP, data));
				SHADE_IF_NEEDED(0.5f);
				t.normal(0.0f, -1.0f, 0.0f);
				renderFaceUp(tile, Vec3::ZERO, tile->getTexture(Facing::DOWN, data));
				SHADE_IF_NEEDED(0.8f);
				t.normal(0.0f, 0.0f, -1.0f);
				renderNorth(tile, Vec3::ZERO, tile->getTexture(Facing::NORTH, data));
				t.normal(0.0f, 0.0f, 1.0f);
				renderSouth(tile, Vec3::ZERO, tile->getTexture(Facing::SOUTH, data));
				SHADE_IF_NEEDED(0.6f);
				t.normal(-1.0f, 0.0f, 0.0f);
				renderWest(tile, Vec3::ZERO, tile->getTexture(Facing::WEST, data));
				t.normal(1.0f, 0.0f, 0.0f);
				renderEast(tile, Vec3::ZERO, tile->getTexture(Facing::EAST, data));
				SHADE_IF_NEEDED(1.0f);
				t.draw();
			}
			t.addOffset(0.5f, 0.5f, 0.5f);
			tile->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
			break;
		}
	}
}

#ifdef ENH_USE_OWN_AO
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
		ETILE_FACE_COUNT,
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

		if (!m_bDisableCulling && !tile->shouldRenderFace(m_pLevelSource, tp, (Facing::Name)dir))
			continue;

		bBother = true;
		break;
	}

	if (!bBother)
		return false;

	float topR = r, topG = g, topB = b;

	if (tile == Tile::grass)
		r = g = b = 1.0f;

	//Tesselator& t = Tesselator::instance;

	//float fLightHere = tile->getBrightness(m_pLevelSource, pos);

	float lights[ETILE_FACE_COUNT];

	// Get the brightness of the tile we're processing, as well as all tiles around it
	lights[ETILE_FACE_HERE] = tile->getBrightness(m_pLevelSource, pos);

	for (int i = 1; i < ETILE_FACE_COUNT; i++)
	{
		lights[i] = tile->getBrightness(m_pLevelSource, TilePos(pos.x + diffEX[i], pos.y + diffEY[i], pos.z + diffEZ[i]));
	}

	// Render all the faces.
	for (int dir = Facing::DOWN; dir <= Facing::EAST; dir++)
	{
		TilePos tp(pos.x + diffX[dir],
			pos.y + diffY[dir],
			pos.z + diffZ[dir]);

		// check if we should bother in the first place
		if (!m_bDisableCulling && !tile->shouldRenderFace(m_pLevelSource, tp, (Facing::Name)dir))
		{
			continue;
		}

		float fR = r, fG = g, fB = b;
		if (dir == Facing::UP)
			fR = topR, fG = topG, fB = topB;

		for (int i = 0; i < 4; i++)
			m_vtxRed[i] = m_vtxGreen[i] = m_vtxBlue[i] = 1.0f;

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
		}

		for (int i = 0; i < 4; i++)
		{
			m_vtxRed  [i] *= fR * lightingMult[dir];
			m_vtxGreen[i] *= fG * lightingMult[dir];
			m_vtxBlue [i] *= fB * lightingMult[dir];
		}

		m_bAmbientOcclusion = true;

		switch (dir) 
		{
			case Facing::DOWN:
				renderFaceUp  (tile, pos, tile->getTexture(m_pLevelSource, pos, Facing::DOWN));
				break;
			case Facing::UP:
				renderFaceDown(tile, pos, tile->getTexture(m_pLevelSource, pos, Facing::UP));
				break;
			case Facing::NORTH:
				renderNorth   (tile, pos, tile->getTexture(m_pLevelSource, pos, Facing::NORTH));
				break;
			case Facing::SOUTH:
				renderSouth   (tile, pos, tile->getTexture(m_pLevelSource, pos, Facing::SOUTH));
				break;
			case Facing::WEST:
				renderWest    (tile, pos, tile->getTexture(m_pLevelSource, pos, Facing::WEST));
				break;
			case Facing::EAST:
				renderEast    (tile, pos, tile->getTexture(m_pLevelSource, pos, Facing::EAST));
				break;
		}

		if (TileRenderer::m_bFancyGrass && tile->getTexture(m_pLevelSource, pos, (Facing::Name)dir) == TEXTURE_GRASS_SIDE && (dir == Facing::WEST || dir == Facing::EAST || dir == Facing::NORTH || dir == Facing::SOUTH))
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

				m_vtxRed[i] = br;
				m_vtxGreen[i] = br;
				m_vtxBlue[i] = br;
			}

			for (int i = 0; i < 4; i++)
			{
				m_vtxRed[i] *= topR * lightingMult[dir];
				m_vtxGreen[i] *= topG * lightingMult[dir];
				m_vtxBlue[i] *= topB * lightingMult[dir];
			}

			switch (dir) 
			{
				case Facing::DOWN:
					renderFaceUp(tile, pos, TEXTURE_NONE84);
					break;
				case Facing::UP:
					renderFaceDown(tile, pos, TEXTURE_NONE84);
					break;
				case Facing::NORTH:
					renderNorth(tile, pos, TEXTURE_NONE84);
					break;
				case Facing::SOUTH:
					renderSouth(tile, pos, TEXTURE_NONE84);
					break;
				case Facing::WEST:
					renderWest(tile, pos, TEXTURE_NONE84);
					break;
				case Facing::EAST:
					renderEast(tile, pos, TEXTURE_NONE84);
					break;
			}
		}

		m_bAmbientOcclusion = false;
	}

	return true;
}
#endif

int TileRenderer::getTileColor(Tile* tile, const TilePos& pos)
{
	if (tile == nullptr)
	{
		return 0xffffff;
	}

	if ((tile == Tile::grass || tile == Tile::tallGrass) && GrassColor::isAvailable() && m_bBiomeColors)
	{
		m_pLevelSource->getBiomeSource()->getBiomeBlock(pos, 1, 1);
		return GrassColor::get(m_pLevelSource->getBiomeSource()->field_4[0], m_pLevelSource->getBiomeSource()->field_8[0]);
	}
	if (tile == Tile::leaves && FoliageColor::isAvailable() && m_bBiomeColors)
	{
		TileData data = m_pLevelSource->getData(pos);

		if ((data & 1) == 1)
		{
			return FoliageColor::getEvergreenColor();
		}
		if ((data & 2) == 2)
		{
			return FoliageColor::getBirchColor();
		}

		m_pLevelSource->getBiomeSource()->getBiomeBlock(pos, 1, 1);
		return FoliageColor::get(m_pLevelSource->getBiomeSource()->field_4[0], m_pLevelSource->getBiomeSource()->field_8[0]);
	}

	return tile->getColor(m_pLevelSource, pos);
}

bool TileRenderer::useAmbientOcclusion() const
{
	return Minecraft::useAmbientOcclusion;
}
