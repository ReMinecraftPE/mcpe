/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TileRenderer.hpp"
#include "Minecraft.hpp"

TileRenderer::TileRenderer()
{
#ifndef ORIGINAL_CODE
	// @BUG: Not initializing level source
	m_pLevelSource = nullptr;
#endif
}

TileRenderer::TileRenderer(LevelSource* pLevelSource)
{
	m_pLevelSource = pLevelSource;
}

float TileRenderer::getWaterHeight(int x, int y, int z, const Material* pCheckMtl)
{
	int iBias = 0;
	float fHeight = 0.0f;
	for (int i = 0; i < 4; i++)
	{
		int checkX = x - (i & 1);
		int checkY = y;
		int checkZ = z - ((i >> 1) & 1);

		if (m_pLevelSource->getMaterial(checkX, checkY + 1, checkZ) == pCheckMtl)
			return 1.0f;

		Material* pMtl = m_pLevelSource->getMaterial(checkX, checkY, checkZ);
		if (pMtl == pCheckMtl)
		{
			int data = m_pLevelSource->getData(checkX, checkY, checkZ);
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
	return renderShape == SHAPE_SOLID || renderShape == SHAPE_STAIRS;
}

// @NOTE: This sucks! Very badly! But it's how they did it.
void TileRenderer::renderEast(Tile* tile, float x, float y, float z, int texture)
{
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
		t.vertexUV(aabb.max.x + x, aabb.min.y + y, aabb.max.z + z, texU_l, texV_d);
		t.color(m_vtxRed[1], m_vtxGreen[1], m_vtxBlue[1]);
		t.vertexUV(aabb.max.x + x, aabb.min.y + y, aabb.min.z + z, texU_r, texV_d);
		t.color(m_vtxRed[2], m_vtxGreen[2], m_vtxBlue[2]);
		t.vertexUV(aabb.max.x + x, aabb.max.y + y, aabb.min.z + z, texU_r, texV_u);
		t.color(m_vtxRed[3], m_vtxGreen[3], m_vtxBlue[3]);
		t.vertexUV(aabb.max.x + x, aabb.max.y + y, aabb.max.z + z, texU_l, texV_u);
		return;
	}
	
	t.vertexUV(aabb.max.x + x, aabb.min.y + y, aabb.max.z + z, texU_l, texV_d);
	t.vertexUV(aabb.max.x + x, aabb.min.y + y, aabb.min.z + z, texU_r, texV_d);
	t.vertexUV(aabb.max.x + x, aabb.max.y + y, aabb.min.z + z, texU_r, texV_u);
	t.vertexUV(aabb.max.x + x, aabb.max.y + y, aabb.max.z + z, texU_l, texV_u);
}

void TileRenderer::renderWest(Tile* tile, float x, float y, float z, int texture)
{
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
		t.vertexUV(aabb.min.x + x, aabb.max.y + y, aabb.max.z + z, texU_r, texV_u);
		t.color(m_vtxRed[1], m_vtxGreen[1], m_vtxBlue[1]);
		t.vertexUV(aabb.min.x + x, aabb.max.y + y, aabb.min.z + z, texU_l, texV_u);
		t.color(m_vtxRed[2], m_vtxGreen[2], m_vtxBlue[2]);
		t.vertexUV(aabb.min.x + x, aabb.min.y + y, aabb.min.z + z, texU_l, texV_d);
		t.color(m_vtxRed[3], m_vtxGreen[3], m_vtxBlue[3]);
		t.vertexUV(aabb.min.x + x, aabb.min.y + y, aabb.max.z + z, texU_r, texV_d);
		return;
	}
	
	t.vertexUV(aabb.min.x + x, aabb.max.y + y, aabb.max.z + z, texU_r, texV_u);
	t.vertexUV(aabb.min.x + x, aabb.max.y + y, aabb.min.z + z, texU_l, texV_u);
	t.vertexUV(aabb.min.x + x, aabb.min.y + y, aabb.min.z + z, texU_l, texV_d);
	t.vertexUV(aabb.min.x + x, aabb.min.y + y, aabb.max.z + z, texU_r, texV_d);
}

void TileRenderer::renderSouth(Tile* tile, float x, float y, float z, int texture)
{
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
		t.vertexUV(aabb.min.x + x, aabb.max.y + y, aabb.max.z + z, texU_l, texV_u);
		t.color(m_vtxRed[1], m_vtxGreen[1], m_vtxBlue[1]);
		t.vertexUV(aabb.min.x + x, aabb.min.y + y, aabb.max.z + z, texU_l, texV_d);
		t.color(m_vtxRed[2], m_vtxGreen[2], m_vtxBlue[2]);
		t.vertexUV(aabb.max.x + x, aabb.min.y + y, aabb.max.z + z, texU_r, texV_d);
		t.color(m_vtxRed[3], m_vtxGreen[3], m_vtxBlue[3]);
		t.vertexUV(aabb.max.x + x, aabb.max.y + y, aabb.max.z + z, texU_r, texV_u);
		return;
	}

	t.vertexUV(aabb.min.x + x, aabb.max.y + y, aabb.max.z + z, texU_l, texV_u);
	t.vertexUV(aabb.min.x + x, aabb.min.y + y, aabb.max.z + z, texU_l, texV_d);
	t.vertexUV(aabb.max.x + x, aabb.min.y + y, aabb.max.z + z, texU_r, texV_d);
	t.vertexUV(aabb.max.x + x, aabb.max.y + y, aabb.max.z + z, texU_r, texV_u);
}

void TileRenderer::renderNorth(Tile* tile, float x, float y, float z, int texture)
{
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
		t.vertexUV(aabb.min.x + x, aabb.max.y + y, aabb.min.z + z, texU_r, texV_u);
		t.color(m_vtxRed[1], m_vtxGreen[1], m_vtxBlue[1]);
		t.vertexUV(aabb.max.x + x, aabb.max.y + y, aabb.min.z + z, texU_l, texV_u);
		t.color(m_vtxRed[2], m_vtxGreen[2], m_vtxBlue[2]);
		t.vertexUV(aabb.max.x + x, aabb.min.y + y, aabb.min.z + z, texU_l, texV_d);
		t.color(m_vtxRed[3], m_vtxGreen[3], m_vtxBlue[3]);
		t.vertexUV(aabb.min.x + x, aabb.min.y + y, aabb.min.z + z, texU_r, texV_d);
		return;
	}

	t.vertexUV(aabb.min.x + x, aabb.max.y + y, aabb.min.z + z, texU_r, texV_u);
	t.vertexUV(aabb.max.x + x, aabb.max.y + y, aabb.min.z + z, texU_l, texV_u);
	t.vertexUV(aabb.max.x + x, aabb.min.y + y, aabb.min.z + z, texU_l, texV_d);
	t.vertexUV(aabb.min.x + x, aabb.min.y + y, aabb.min.z + z, texU_r, texV_d);
}

void TileRenderer::renderFaceDown(Tile* tile, float x, float y, float z, int texture)
{
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
		t.vertexUV(aabb.max.x + x, aabb.max.y + y, aabb.max.z + z, texU_2, texV_2);
		t.color(m_vtxRed[1], m_vtxGreen[1], m_vtxBlue[1]);
		t.vertexUV(aabb.max.x + x, aabb.max.y + y, aabb.min.z + z, texU_2, texV_1);
		t.color(m_vtxRed[2], m_vtxGreen[2], m_vtxBlue[2]);
		t.vertexUV(aabb.min.x + x, aabb.max.y + y, aabb.min.z + z, texU_1, texV_1);
		t.color(m_vtxRed[3], m_vtxGreen[3], m_vtxBlue[3]);
		t.vertexUV(aabb.min.x + x, aabb.max.y + y, aabb.max.z + z, texU_1, texV_2);
		return;
	}

	t.vertexUV(aabb.max.x + x, aabb.max.y + y, aabb.max.z + z, texU_2, texV_2);
	t.vertexUV(aabb.max.x + x, aabb.max.y + y, aabb.min.z + z, texU_2, texV_1);
	t.vertexUV(aabb.min.x + x, aabb.max.y + y, aabb.min.z + z, texU_1, texV_1);
	t.vertexUV(aabb.min.x + x, aabb.max.y + y, aabb.max.z + z, texU_1, texV_2);
}

void TileRenderer::renderFaceUp(Tile* tile, float x, float y, float z, int texture)
{
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
		t.vertexUV(aabb.min.x + x, aabb.min.y + y, aabb.max.z + z, texU_1, texV_2);
		t.color(m_vtxRed[1], m_vtxGreen[1], m_vtxBlue[1]);
		t.vertexUV(aabb.min.x + x, aabb.min.y + y, aabb.min.z + z, texU_1, texV_1);
		t.color(m_vtxRed[2], m_vtxGreen[2], m_vtxBlue[2]);
		t.vertexUV(aabb.max.x + x, aabb.min.y + y, aabb.min.z + z, texU_2, texV_1);
		t.color(m_vtxRed[3], m_vtxGreen[3], m_vtxBlue[3]);
		t.vertexUV(aabb.max.x + x, aabb.min.y + y, aabb.max.z + z, texU_2, texV_2);
		return;
	}

	t.vertexUV(aabb.min.x + x, aabb.min.y + y, aabb.max.z + z, texU_1, texV_2);
	t.vertexUV(aabb.min.x + x, aabb.min.y + y, aabb.min.z + z, texU_1, texV_1);
	t.vertexUV(aabb.max.x + x, aabb.min.y + y, aabb.min.z + z, texU_2, texV_1);
	t.vertexUV(aabb.max.x + x, aabb.min.y + y, aabb.max.z + z, texU_2, texV_2);
}

void TileRenderer::tesselateCrossTexture(Tile* tile, int data, float x, float y, float z)
{
	static constexpr float C_RATIO = 1.0f / 256.0f;

	int texture = m_textureOverride;
	if (texture < 0)
		texture = tile->getTexture(DIR_YNEG, data);

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	// calculate U and V coordinates
	float texU_l = texX * C_RATIO, texU_r = (texX + 15.99f) * C_RATIO;
	float texV_u = texY * C_RATIO, texV_d = (texY + 15.99f) * C_RATIO;

	float cenX = x + 0.5f, cenZ = z + 0.5f;
	
	float x1 = cenX - 0.45f, x2 = cenX + 0.45f;
	float z1 = cenZ - 0.45f, z2 = cenZ + 0.45f;

	Tesselator& t = Tesselator::instance;
	// face 1
	t.vertexUV(x1, y + 1, z1, texU_l, texV_u);
	t.vertexUV(x1, y + 0, z1, texU_l, texV_d);
	t.vertexUV(x2, y + 0, z2, texU_r, texV_d);
	t.vertexUV(x2, y + 1, z2, texU_r, texV_u);

	// face 2
	t.vertexUV(x2, y + 1, z2, texU_l, texV_u);
	t.vertexUV(x2, y + 0, z2, texU_l, texV_d);
	t.vertexUV(x1, y + 0, z1, texU_r, texV_d);
	t.vertexUV(x1, y + 1, z1, texU_r, texV_u);

	// face 3
	t.vertexUV(x1, y + 1, z2, texU_l, texV_u);
	t.vertexUV(x1, y + 0, z2, texU_l, texV_d);
	t.vertexUV(x2, y + 0, z1, texU_r, texV_d);
	t.vertexUV(x2, y + 1, z1, texU_r, texV_u);

	// face 4
	t.vertexUV(x2, y + 1, z1, texU_l, texV_u);
	t.vertexUV(x2, y + 0, z1, texU_l, texV_d);
	t.vertexUV(x1, y + 0, z2, texU_r, texV_d);
	t.vertexUV(x1, y + 1, z2, texU_r, texV_u);
}

bool TileRenderer::tesselateBlockInWorld(Tile* tile, int x, int y, int z, float r, float g, float b)
{
	float topR = r, topG = g, topB = b;

	if (tile == Tile::grass)
		r = g = b = 1.0f;

	Tesselator& t = Tesselator::instance;

	float fLightHere = tile->getBrightness(m_pLevelSource, x, y, z);
	bool bDrewAnything = false;

	if (m_bDisableCulling || tile->shouldRenderFace(m_pLevelSource, x, y - 1, z, DIR_YNEG))
	{
		bDrewAnything = true;

		float fLight = tile->getBrightness(m_pLevelSource, x, y - 1, z);
		t.color(r * 0.5f * fLight, g * 0.5f * fLight, b * 0.5f * fLight);

		renderFaceUp(tile, float(x), float(y), float(z), tile->getTexture(m_pLevelSource, x, y, z, DIR_YNEG));
	}

	if (m_bDisableCulling || tile->shouldRenderFace(m_pLevelSource, x, y + 1, z, DIR_YPOS))
	{
		bDrewAnything = true;

		float fLight = tile->getBrightness(m_pLevelSource, x, y + 1, z);
		if (tile->m_aabb.max.y != 1.0f && !tile->m_pMaterial->isLiquid())
			fLight = fLightHere;

		t.color(topR * fLight, topG * fLight, topB * fLight);

		renderFaceDown(tile, float(x), float(y), float(z), tile->getTexture(m_pLevelSource, x, y, z, DIR_YPOS));
	}

	if (m_bDisableCulling || tile->shouldRenderFace(m_pLevelSource, x, y, z - 1, DIR_ZNEG))
	{
		bDrewAnything = true;

		float fLight = tile->getBrightness(m_pLevelSource, x, y, z - 1);
		if (tile->m_aabb.min.z > 0.0f)
			fLight = fLightHere;

		t.color(r * 0.8f * fLight, g * 0.8f * fLight, b * 0.8f * fLight);

		renderNorth(tile, float(x), float(y), float(z), tile->getTexture(m_pLevelSource, x, y, z, DIR_ZNEG));
	}

	if (m_bDisableCulling || tile->shouldRenderFace(m_pLevelSource, x, y, z + 1, DIR_ZPOS))
	{
		bDrewAnything = true;

		float fLight = tile->getBrightness(m_pLevelSource, x, y, z + 1);
		if (tile->m_aabb.max.z < 1.0f)
			fLight = fLightHere;

		t.color(r * 0.8f * fLight, g * 0.8f * fLight, b * 0.8f * fLight);

		renderSouth(tile, float(x), float(y), float(z), tile->getTexture(m_pLevelSource, x, y, z, DIR_ZPOS));
	}

	if (m_bDisableCulling || tile->shouldRenderFace(m_pLevelSource, x - 1, y, z, DIR_XNEG))
	{
		bDrewAnything = true;

		float fLight = tile->getBrightness(m_pLevelSource, x - 1, y, z);
		if (tile->m_aabb.min.x > 0.0f)
			fLight = fLightHere;

		t.color(r * 0.6f * fLight, g * 0.6f * fLight, b * 0.6f * fLight);
		
		renderWest(tile, float(x), float(y), float(z), tile->getTexture(m_pLevelSource, x, y, z, DIR_XNEG));
	}

	if (m_bDisableCulling || tile->shouldRenderFace(m_pLevelSource, x + 1, y, z, DIR_XPOS))
	{
		bDrewAnything = true;

		float fLight = tile->getBrightness(m_pLevelSource, x + 1, y, z);
		if (tile->m_aabb.max.x < 1.0f)
			fLight = fLightHere;

		t.color(r * 0.6f * fLight, g * 0.6f * fLight, b * 0.6f * fLight);

		renderEast(tile, float(x), float(y), float(z), tile->getTexture(m_pLevelSource, x, y, z, DIR_XPOS));
	}

	return bDrewAnything;
}

bool TileRenderer::tesselateBlockInWorld(Tile* tile, int x, int y, int z)
{
	int color = tile->getColor(m_pLevelSource, x, y, z);

	float r = float(GET_RED  (color)) / 255.0f;
	float g = float(GET_GREEN(color)) / 255.0f;
	float b = float(GET_BLUE (color)) / 255.0f;

	if (Minecraft::useAmbientOcclusion)
	{
#ifdef ENH_USE_OWN_AO
		return tesselateBlockInWorldWithAmbienceOcclusionV2(tile, x, y, z, r, g, b);
#else
		return tesselateBlockInWorldWithAmbienceOcclusion(tile, x, y, z, r, g, b);
#endif
	}

	return tesselateBlockInWorld(tile, x, y, z, r, g, b);
}

bool TileRenderer::tesselateCrossInWorld(Tile* tile, int x, int y, int z)
{
	Tesselator& t = Tesselator::instance;

	float bright = tile->getBrightness(m_pLevelSource, x, y, z);

	t.color(bright, bright, bright);

	tesselateCrossTexture(tile, m_pLevelSource->getData(x, y, z), float(x), float(y), float(z));

	return true;
}

bool TileRenderer::tesselateWaterInWorld(Tile* tile1, int x, int y, int z)
{
	constexpr float C_RATIO = 1.0f / 256.0f;

	LiquidTile* tile = (LiquidTile*)tile1;
	bool bRenderFaceDown, bRenderFaceUp, bRenderSides[4];

	Tesselator& t = Tesselator::instance;

	bRenderFaceDown = tile->shouldRenderFace(m_pLevelSource, x, y + 1, z, DIR_YPOS);
	bRenderFaceUp   = tile->shouldRenderFace(m_pLevelSource, x, y - 1, z, DIR_YNEG);

	bRenderSides[0] = tile->shouldRenderFace(m_pLevelSource, x, y, z - 1, DIR_ZNEG);
	bRenderSides[1] = tile->shouldRenderFace(m_pLevelSource, x, y, z + 1, DIR_ZPOS);
	bRenderSides[2] = tile->shouldRenderFace(m_pLevelSource, x - 1, y, z, DIR_XNEG);
	bRenderSides[3] = tile->shouldRenderFace(m_pLevelSource, x + 1, y, z, DIR_XPOS);

	if (!bRenderFaceDown &&
		!bRenderFaceUp   &&
		!bRenderSides[0] &&
		!bRenderSides[1] &&
		!bRenderSides[2] &&
		!bRenderSides[3])
		return false;

	int tileData = m_pLevelSource->getData(x, y, z);
	float
		fHeight1 = getWaterHeight(x,     y, z,     tile->m_pMaterial),
		fHeight2 = getWaterHeight(x,     y, z + 1, tile->m_pMaterial),
		fHeight3 = getWaterHeight(x + 1, y, z + 1, tile->m_pMaterial),
		fHeight4 = getWaterHeight(x + 1, y, z,     tile->m_pMaterial);

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
		int texFaceDown = tile->getTexture(DIR_YPOS, tileData);
		float slopeAngle = tile->getSlopeAngle(m_pLevelSource, x, y, z, tile->m_pMaterial);

		int texX, texY;

		if (slopeAngle > -999.0f)
		{
			int texNorth = tile->getTexture(DIR_ZNEG, tileData);
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

		float bright = tile->getBrightness(m_pLevelSource, x, y, z);

		texUV_5 = texUV_1 - texUV_4;
		texUV_6 = texUV_2 - texUV_4;

		t.color(bright, bright, bright);
		texUV_7 = texUV_2 + texUV_4;
		texUV_8 = texUV_1 + texUV_4;

		t.vertexUV(x + 0.0f, y + fHeight1, z + 0.0f, (texUV_1 - texUV_4) - texUV_3, texUV_6 + texUV_3);
		t.vertexUV(x + 0.0f, y + fHeight2, z + 1.0f, texUV_3 + texUV_5, texUV_7 + texUV_3);
		t.vertexUV(x + 1.0f, y + fHeight3, z + 1.0f, texUV_8 + texUV_3, texUV_7 - texUV_3);
		t.vertexUV(x + 1.0f, y + fHeight4, z + 0.0f, texUV_8 - texUV_3, texUV_6 - texUV_3);
	}

	if (m_bDisableCulling)
		goto label_7;

	bFlag1 = true;
	if (bRenderFaceUp)
	{
	label_6:
	label_7:
		float bright = tile->getBrightness(m_pLevelSource, x, y - 1, z);
		t.color(bright * 0.5f, bright * 0.5f, bright * 0.5f);
		renderFaceUp(tile1, float(x), float(y), float(z), tile->getTexture(DIR_YNEG));
		bFlag1 = true;
	}

label_8:
	bFlag2 = bFlag1;
	bool bRenderedSides = false;

	for (int dir = 0; dir < 4; dir++)
	{
		int checkX = x, checkZ = z;
		switch (dir)
		{
			case 0: checkZ--; break;
			case 1: checkZ++; break;
			case 2: checkX--; break;
			case 3: checkX++; break;
		}

		int texture = tile1->getTexture(dir + DIR_ZNEG, tileData);
		if (!m_bDisableCulling && !bRenderSides[dir])
			continue;

		float vtxX1, vtxX2, vtxZ1, vtxZ2, height1, height2;
		switch (dir)
		{
			case 0:
			{
				vtxX1 = float(x);
				vtxX2 = float(x + 1);
				vtxZ1 = float(z);
				vtxZ2 = float(z);
				height1 = fHeight1;
				height2 = fHeight4;
				break;
			}
			case 1:
			{
				vtxX1 = float(x + 1);
				vtxX2 = float(x);
				vtxZ1 = float(z + 1);
				vtxZ2 = float(z + 1);
				height1 = fHeight3;
				height2 = fHeight2;
				break;
			}
			case 2:
			{
				vtxX1 = float(x);
				vtxX2 = float(x);
				vtxZ1 = float(z + 1);
				vtxZ2 = float(z);
				height1 = fHeight2;
				height2 = fHeight1;
				break;
			}
			case 3:
			{
				vtxX1 = float(x + 1);
				vtxX2 = float(x + 1);
				vtxZ1 = float(z);
				vtxZ2 = float(z + 1);
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

		float brightMul = dir >= DIR_XNEG ? 0.6f : 0.8f;
		float bright = tile->getBrightness(m_pLevelSource, checkX, y, checkZ);
		t.color(bright* brightMul, bright* brightMul, bright* brightMul);
		t.vertexUV(vtxX1, float(y) + height1, vtxZ1, texU_1, texV_1);
		t.vertexUV(vtxX2, float(y) + height2, vtxZ2, texU_2, texV_2);
		t.vertexUV(vtxX2, float(y) + 0.0f, vtxZ2, texU_2, texV_3);
		t.vertexUV(vtxX1, float(y) + 0.0f, vtxZ1, texU_1, texV_3);
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

bool TileRenderer::tesselateStairsInWorld(Tile* tile, int x, int y, int z)
{
	bool bRenderedAnything = false;

	switch (m_pLevelSource->getData(x, y, z))
	{
		case 0:
		{
			tile->setShape(0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 1.0f);
			PROC_RESULT(tesselateBlockInWorld(tile, x, y, z));
			tile->setShape(0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
			PROC_RESULT(tesselateBlockInWorld(tile, x, y, z));
			break;
		}
		case 1:
		{
			tile->setShape(0.0f, 0.0f, 0.0f, 0.5f, 1.0f, 1.0f);
			PROC_RESULT(tesselateBlockInWorld(tile, x, y, z));
			tile->setShape(0.5f, 0.0f, 0.0f, 1.0f, 0.5f, 1.0f);
			PROC_RESULT(tesselateBlockInWorld(tile, x, y, z));
			break;
		}
		case 2:
		{
			tile->setShape(0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f);
			PROC_RESULT(tesselateBlockInWorld(tile, x, y, z));
			tile->setShape(0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f);
			PROC_RESULT(tesselateBlockInWorld(tile, x, y, z));
			break;
		}
		case 3:
		{
			tile->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f);
			PROC_RESULT(tesselateBlockInWorld(tile, x, y, z));
			tile->setShape(0.0f, 0.0f, 0.5f, 1.0f, 0.5f, 1.0f);
			PROC_RESULT(tesselateBlockInWorld(tile, x, y, z));
			break;
		}
	}

	tile->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	// @BUG: In a chunk that only contains stairs, everything will be invisible, because this
	// function returns 0.
	return bRenderedAnything;
}

bool TileRenderer::tesselateDoorInWorld(Tile* tile, int x, int y, int z)
{
	Tesselator& t = Tesselator::instance;
	float fBrightHere = tile->getBrightness(m_pLevelSource, x, y, z), fBright;
	int texture;

	fBright = tile->getBrightness(m_pLevelSource, x, y - 1, z);
	if (tile->m_aabb.min.y > 0.0f)       fBright = fBrightHere;
	if (Tile::lightEmission[tile->m_ID]) fBright = 1.0f;
	t.color(fBright * 0.5f, fBright * 0.5f, fBright * 0.5f);
	renderFaceUp(tile, float(x), float(y), float(z), tile->getTexture(m_pLevelSource, x, y, z, DIR_YNEG));

	fBright = tile->getBrightness(m_pLevelSource, x, y + 1, z);
	if (tile->m_aabb.max.y < 1.0f)       fBright = fBrightHere;
	if (Tile::lightEmission[tile->m_ID]) fBright = 1.0f;
	t.color(fBright, fBright, fBright);
	renderFaceDown(tile, float(x), float(y), float(z), tile->getTexture(m_pLevelSource, x, y, z, DIR_YPOS));

	fBright = tile->getBrightness(m_pLevelSource, x, y, z - 1);
	if (tile->m_aabb.min.z > 0.0f)       fBright = fBrightHere;
	if (Tile::lightEmission[tile->m_ID]) fBright = 1.0f;
	t.color(fBright * 0.8f, fBright * 0.8f, fBright * 0.8f);
	texture = tile->getTexture(m_pLevelSource, x, y, z, DIR_ZNEG);
	if (texture < 0) texture = -texture, field_8 = true;
	renderNorth(tile, float(x), float(y), float(z), texture);
	field_8 = false;

	fBright = tile->getBrightness(m_pLevelSource, x, y, z + 1);
	if (tile->m_aabb.max.z < 1.0f)       fBright = fBrightHere;
	if (Tile::lightEmission[tile->m_ID]) fBright = 1.0f;
	t.color(fBright * 0.8f, fBright * 0.8f, fBright * 0.8f);
	texture = tile->getTexture(m_pLevelSource, x, y, z, DIR_ZPOS);
	if (texture < 0) texture = -texture, field_8 = true;
	renderSouth(tile, float(x), float(y), float(z), texture);
	field_8 = false;

	fBright = tile->getBrightness(m_pLevelSource, x - 1, y, z);
	if (tile->m_aabb.min.x > 0.0f)       fBright = fBrightHere;
	if (Tile::lightEmission[tile->m_ID]) fBright = 1.0f;
	t.color(fBright * 0.6f, fBright * 0.6f, fBright * 0.6f);
	texture = tile->getTexture(m_pLevelSource, x, y, z, DIR_XNEG);
	if (texture < 0) texture = -texture, field_8 = true;
	renderWest(tile, float(x), float(y), float(z), texture);
	field_8 = false;

	fBright = tile->getBrightness(m_pLevelSource, x + 1, y, z);
	if (tile->m_aabb.max.x < 1.0f)       fBright = fBrightHere;
	if (Tile::lightEmission[tile->m_ID]) fBright = 1.0f;
	t.color(fBright * 0.6f, fBright * 0.6f, fBright * 0.6f);
	texture = tile->getTexture(m_pLevelSource, x, y, z, DIR_XPOS);
	if (texture < 0) texture = -texture, field_8 = true;
	renderEast(tile, float(x), float(y), float(z), texture);
	field_8 = false;

	return true;
}

void TileRenderer::tesselateTorch(Tile* tile, float x, float y, float z, float a, float b)
{
	constexpr float C_RATIO = 1.0f / 256.0f;
	constexpr float C_ONE_PIXEL = 1.0f / 16.0f;
	constexpr float C_HALF_TILE = 1.0f / 2.0f;
	constexpr float C_UNK_1 = 0.375f;

	int texture = tile->getTexture(DIR_YNEG);

	if (m_textureOverride >= 0)
		texture = m_textureOverride;

	// @TODO: Clean up a bit

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	float texU_1 = texX * C_RATIO;
	float texU_2 = (texX + 15.99f) * C_RATIO;
	float texV_1 = (texY * C_RATIO) + 1.0f / 32.0f;
	float texV_2 = (texY + 15.99f) * C_RATIO;

	float x1 = x + 0.5f, z1 = z + 0.5f;
	float z2 = z1 + (float)(b * C_UNK_1);

	Tesselator& t = Tesselator::instance;

	float x_1 = (x1 + (a * C_UNK_1)) - C_ONE_PIXEL;
	float x_2 = (x1 + (a * C_UNK_1)) + C_ONE_PIXEL;
	float x_3 = x1 - C_ONE_PIXEL;
	float x_4 = x1 + C_ONE_PIXEL;
	float x_5 = x1 - C_HALF_TILE;
	float x_6 = x1 + C_HALF_TILE;
	float x_7 = x_6 + a;
	float x_8 = x_3 + a;
	float x_9 = x_4 + a;
	float x_0 = x_5 + a;

	float y_1 = y + C_ONE_PIXEL * 10.0f;
	float y_2 = y + 1.0f;
	float y_3 = y + 0.0f;

	float z_1 = z2 - C_ONE_PIXEL;
	float z_2 = z2 + C_ONE_PIXEL;
	float z_3 = z1 - 0.5f;
	float z_4 = z1 + 0.5f;
	float z_5 = z1 - C_ONE_PIXEL;
	float z_6 = z1 + C_ONE_PIXEL;
	float z_7 = z_3 + b;
	float z_8 = z_4 + b;
	float z_9 = z_5 + b;
	float z_0 = z_6 + b;

	float texU_3 = texU_1 + 0.027344f;
	float texU_4 = texU_1 + 0.035156f;
	float texV_3 = texY * C_RATIO;
	float texV_4 = texY * C_RATIO + 0.023438f;

	t.vertexUV(x_1, y_1, z_1, texU_3, texV_4);
	t.vertexUV(x_1, y_1, z_2, texU_3, texV_1);
	t.vertexUV(x_2, y_1, z_2, texU_4, texV_1);
	t.vertexUV(x_2, y_1, z_1, texU_4, texV_4);
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

bool TileRenderer::tesselateTorchInWorld(Tile* tile, int x, int y, int z)
{
	int data = m_pLevelSource->getData(x, y, z);
	float bright = tile->getBrightness(m_pLevelSource, x, y, z);

	if (Tile::lightEmission[tile->m_ID] > 0)
		bright = 1.0f;

	Tesselator& t = Tesselator::instance;
	t.color(bright, bright, bright);

	switch (data)
	{
		case 1:
			tesselateTorch(tile, float(x) - 0.1f, float(y) + 0.2f, float(z), -0.4f, 0.0f);
			break;
		case 2:
			tesselateTorch(tile, float(x) + 0.1f, float(y) + 0.2f, float(z), 0.4f, 0.0f);
			break;
		case 3:
			tesselateTorch(tile, float(x), float(y) + 0.2f, float(z) - 0.1f, 0.0f, -0.4f);
			break;
		case 4:
			tesselateTorch(tile, float(x), float(y) + 0.2f, float(z) + 0.1f, 0.0f, 0.4f);
			break;
		default:
			tesselateTorch(tile, float(x), float(y), float(z), 0.0f, 0.0f);
			break;
	}

	return true;
}

bool TileRenderer::tesselateLadderInWorld(Tile* tile, int x, int y, int z)
{
	constexpr float C_RATIO = 1.0f / 256.0f;

	Tesselator& t = Tesselator::instance;

	int texture = tile->getTexture(DIR_YNEG);

	if (m_textureOverride >= 0)
		texture = m_textureOverride;

	float bright = m_pLevelSource->getBrightness(x, y, z);
	t.color(bright, bright, bright);

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	float texU_1 = C_RATIO * texX;
	float texU_2 = C_RATIO * (texX + 15.99f);
	float texV_1 = C_RATIO * texY;
	float texV_2 = C_RATIO * (texY + 15.99f);

	switch (m_pLevelSource->getData(x, y, z))
	{
		case 2:
			t.vertexUV(float(x + 1), float(y + 1), float(z + 1) - 0.05f, texU_1, texV_1);
			t.vertexUV(float(x + 1), float(y + 0), float(z + 1) - 0.05f, texU_1, texV_2);
			t.vertexUV(float(x + 0), float(y + 0), float(z + 1) - 0.05f, texU_2, texV_2);
			t.vertexUV(float(x + 0), float(y + 1), float(z + 1) - 0.05f, texU_2, texV_1);
			break;
		case 3:
			t.vertexUV(float(x + 1), float(y + 0), float(z) + 0.05f, texU_2, texV_2);
			t.vertexUV(float(x + 1), float(y + 1), float(z) + 0.05f, texU_2, texV_1);
			t.vertexUV(float(x + 0), float(y + 1), float(z) + 0.05f, texU_1, texV_1);
			t.vertexUV(float(x + 0), float(y + 0), float(z) + 0.05f, texU_1, texV_2);
			break;
		case 4:
			t.vertexUV(float(x + 1) - 0.05f, float(y + 0), float(z + 1), texU_2, texV_2);
			t.vertexUV(float(x + 1) - 0.05f, float(y + 1), float(z + 1), texU_2, texV_1);
			t.vertexUV(float(x + 1) - 0.05f, float(y + 1), float(z + 0), texU_1, texV_1);
			t.vertexUV(float(x + 1) - 0.05f, float(y + 0), float(z + 0), texU_1, texV_2);
			break;
		case 5:
			t.vertexUV(float(x) + 0.05f, float(y + 1), float(z + 1), texU_1, texV_1);
			t.vertexUV(float(x) + 0.05f, float(y + 0), float(z + 1), texU_1, texV_2);
			t.vertexUV(float(x) + 0.05f, float(y + 0), float(z + 0), texU_2, texV_2);
			t.vertexUV(float(x) + 0.05f, float(y + 1), float(z + 0), texU_2, texV_1);
			break;
	}

	return true;
}

bool TileRenderer::tesselateFireInWorld(Tile* tile, int x, int y, int z)
{
	constexpr float C_RATIO = 1.0f / 256.0f;

	Tesselator& t = Tesselator::instance;

	int texture = tile->getTexture(DIR_YNEG);
	float bright = tile->getBrightness(m_pLevelSource, x, y, z);

	t.color(bright, bright, bright);

	FireTile* pFireTile = (FireTile*)Tile::fire;

	float texX = float(16 * (texture % 16));
	float texY = float(16 * (texture / 16));

	float texU_1 = C_RATIO * (texX);
	float texU_2 = C_RATIO * (texX + 15.99f);
	float texV_1 = C_RATIO * (texY);
	float texV_2 = C_RATIO * (texY + 15.99f);
	float xf = float(x), yf = float(y), zf = float(z);

	if (m_pLevelSource->isSolidTile(x, y - 1, z) || pFireTile->canBurn(m_pLevelSource, x, y - 1, z))
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

	if (((y / 2 + x / 2 + z / 2) & 1) == 0)
		std::swap(texU_1, texU_2);

	if (pFireTile->canBurn(m_pLevelSource, x - 1, y, z))
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

	if (pFireTile->canBurn(m_pLevelSource, x + 1, y, z))
	{
		t.vertexUV(xf + 1.0f - 0.2f, (y + 1.4f) + 0.0625f, z + 0.0f, texU_2, texV_1);
		t.vertexUV(xf + 1.0f + 0.0f, (y + 0.0f) + 0.0625f, z + 0.0f, texU_2, texV_2);
		t.vertexUV(xf + 1.0f + 0.0f, (y + 0.0f) + 0.0625f, z + 1.0f, texU_1, texV_2);
		t.vertexUV(xf + 1.0f - 0.2f, (y + 1.4f) + 0.0625f, z + 1.0f, texU_1, texV_1);
		t.vertexUV(xf + 1.0f - 0.2f, (y + 1.4f) + 0.0625f, z + 1.0f, texU_1, texV_1);
		t.vertexUV(xf + 1.0f + 0.0f, (y + 0.0f) + 0.0625f, z + 1.0f, texU_1, texV_2);
		t.vertexUV(xf + 1.0f + 0.0f, (y + 0.0f) + 0.0625f, z + 0.0f, texU_2, texV_2);
		t.vertexUV(xf + 1.0f - 0.2f, (y + 1.4f) + 0.0625f, z + 0.0f, texU_2, texV_1);
	}

	if (pFireTile->canBurn(m_pLevelSource, x, y, z - 1))
	{
		t.vertexUV(x + 0.0f, (y + 1.4f) + 0.0625f, z + 0.2f, texU_1, texV_1);
		t.vertexUV(x + 0.0f, (y + 0.0f) + 0.0625f, z + 0.0f, texU_1, texV_2);
		t.vertexUV(x + 1.0f, (y + 0.0f) + 0.0625f, z + 0.0f, texU_2, texV_2);
		t.vertexUV(x + 1.0f, (y + 1.4f) + 0.0625f, z + 0.2f, texU_2, texV_1);
		t.vertexUV(x + 1.0f, (y + 1.4f) + 0.0625f, z + 0.2f, texU_2, texV_1);
		t.vertexUV(x + 1.0f, (y + 0.0f) + 0.0625f, z + 0.0f, texU_2, texV_2);
		t.vertexUV(x + 0.0f, (y + 0.0f) + 0.0625f, z + 0.0f, texU_1, texV_2);
		t.vertexUV(x + 0.0f, (y + 1.4f) + 0.0625f, z + 0.2f, texU_1, texV_1);
	}

	if (pFireTile->canBurn(m_pLevelSource, x, y, z + 1))
	{
		t.vertexUV(x + 1.0f, (y + 1.4f) + 0.0625f, z + 1.0f - 0.2f, texU_2, texV_1);
		t.vertexUV(x + 1.0f, (y + 0.0f) + 0.0625f, z + 1.0f + 0.0f, texU_2, texV_2);
		t.vertexUV(x + 0.0f, (y + 0.0f) + 0.0625f, z + 1.0f + 0.0f, texU_1, texV_2);
		t.vertexUV(x + 0.0f, (y + 1.4f) + 0.0625f, z + 1.0f - 0.2f, texU_1, texV_1);
		t.vertexUV(x + 0.0f, (y + 1.4f) + 0.0625f, z + 1.0f - 0.2f, texU_1, texV_1);
		t.vertexUV(x + 0.0f, (y + 0.0f) + 0.0625f, z + 1.0f + 0.0f, texU_1, texV_2);
		t.vertexUV(x + 1.0f, (y + 0.0f) + 0.0625f, z + 1.0f + 0.0f, texU_2, texV_2);
		t.vertexUV(x + 1.0f, (y + 1.4f) + 0.0625f, z + 1.0f - 0.2f, texU_2, texV_1);
	}

	if (pFireTile->canBurn(m_pLevelSource, x, y + 1, z))
	{
		// @NOTE: Converting z and x to uint8_t for whatever reason
		if (((uint8_t(z) + uint8_t(x) + y + 1) & 1) != 0)
		{
			float y1f = float(y + 1);
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
			float y1f = float(y + 1);
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

bool TileRenderer::tesselateInWorld(Tile* tile, int x, int y, int z)
{
	int shape = tile->getRenderShape();
	tile->updateShape(m_pLevelSource, x, y, z);

	switch (shape)
	{
		// @NOTE: In the original Minecraft PE, if a shape is undefined, the tile is invisible.
		// @NOTE: In v0.1.0, fire is invisible.
#ifndef ORIGINAL_CODE
		case SHAPE_NONE:
			return false;
		case SHAPE_FIRE:
			return tesselateFireInWorld(tile, x, y, z);
		default:
#endif
		case SHAPE_SOLID:
			return tesselateBlockInWorld(tile, x, y, z);
		case SHAPE_WATER:
			return tesselateWaterInWorld(tile, x, y, z);
		case SHAPE_CROSS:
			return tesselateCrossInWorld(tile, x, y, z);
		case SHAPE_TORCH:
			return tesselateTorchInWorld(tile, x, y, z);
		case SHAPE_LADDER:
			return tesselateLadderInWorld(tile, x, y, z);
		case SHAPE_DOOR:
			return tesselateDoorInWorld(tile, x, y, z);
		case SHAPE_STAIRS:
			return tesselateStairsInWorld(tile, x, y, z);
	}

	return false;
}

bool TileRenderer::tesselateInWorldNoCulling(Tile* tile, int x, int y, int z)
{
	m_bDisableCulling = true;
	bool r = tesselateInWorld(tile, x, y, z);
	m_bDisableCulling = false;
	return r;
}

bool TileRenderer::tesselateInWorld(Tile* tile, int x, int y, int z, int a)
{
	m_textureOverride = a;
	bool r = tesselateInWorld(tile, x, y, z);
	m_textureOverride = -1;
	return r;
}

bool TileRenderer::tesselateBlockInWorldWithAmbienceOcclusion(Tile* a2, int a3, int a4, int a5, float a6, float a7, float a8)
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

	this->m_bAmbientOcclusion = 1;
	v12 = a2->getBrightness(this->m_pLevelSource, a3, a4, a5);
	v13 = this->m_pLevelSource;
	this->field_C = v12;
	v14 = a2->getBrightness(v13, a3 - 1, a4, a5);
	v218 = a4 - 1;
	v15 = this->m_pLevelSource;
	this->field_10 = v14;
	v16 = a2->getBrightness(v15, a3, a4 - 1, a5);
	v217 = a5 - 1;
	v17 = this->m_pLevelSource;
	this->field_14 = v16;
	v18 = a2->getBrightness(v17, a3, a4, a5 - 1);
	v214 = a3 + 1;
	v19 = this->m_pLevelSource;
	this->field_18 = v18;
	v20 = a2->getBrightness(v19, a3 + 1, a4, a5);
	v216 = a4 + 1;
	v21 = this->m_pLevelSource;
	this->field_1C = v20;
	v22 = a2->getBrightness(v21, a3, a4 + 1, a5);
	v215 = a5 + 1;
	v23 = this->m_pLevelSource;
	this->field_20 = v22;
	this->field_24 = a2->getBrightness(v23, a3, a4, a5 + 1);
	v24 = Tile::translucent[this->m_pLevelSource->getTile(a3 + 1, a4 + 1, a5)];
	v25 = this->m_pLevelSource;
	this->field_AD = v24;
	v26 = Tile::translucent[v25->getTile(a3 + 1, a4 - 1, a5)];
	v27 = this->m_pLevelSource;
	this->field_B5 = v26;
	v28 = Tile::translucent[v27->getTile(a3 + 1, a4, a5 + 1)];
	v29 = this->m_pLevelSource;
	this->field_B1 = v28;
	v30 = Tile::translucent[v29->getTile(a3 + 1, a4, a5 - 1)];
	v31 = this->m_pLevelSource;
	this->field_B3 = v30;
	v32 = Tile::translucent[v31->getTile(a3 - 1, a4 + 1, a5)];
	v33 = this->m_pLevelSource;
	this->field_AE = v32;
	v34 = Tile::translucent[v33->getTile(a3 - 1, a4 - 1, a5)];
	v35 = this->m_pLevelSource;
	this->field_B6 = v34;
	v36 = Tile::translucent[v35->getTile(a3 - 1, a4, a5 - 1)];
	v37 = this->m_pLevelSource;
	this->field_B0 = v36;
	v38 = Tile::translucent[v37->getTile(a3 - 1, a4, a5 + 1)];
	v39 = this->m_pLevelSource;
	this->field_B2 = v38;
	v40 = Tile::translucent[v39->getTile(a3, a4 + 1, a5 + 1)];
	v41 = this->m_pLevelSource;
	this->field_AF = v40;
	v42 = Tile::translucent[v41->getTile(a3, a4 + 1, a5 - 1)];
	v43 = this->m_pLevelSource;
	this->field_AC = v42;
	v44 = Tile::translucent[v43->getTile(a3, a4 - 1, a5 + 1)];
	v45 = this->m_pLevelSource;
	this->field_B7 = v44;
	this->field_B4 = Tile::translucent[v45->getTile(a3, a4 - 1, a5 - 1)];
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
		&& !a2->shouldRenderFace(this->m_pLevelSource, a3, v218, a5, this->m_bDisableCulling))
	{
		v69 = 0;
		goto LABEL_20;
	}
	if (this->field_78 > 0)
	{
		v47 = a2->getBrightness(this->m_pLevelSource, a3 - 1, v218, a5);
		v48 = this->m_pLevelSource;
		this->field_2C = v47;
		v49 = a2->getBrightness(v48, a3, v218, v217);
		v50 = this->m_pLevelSource;
		this->field_34 = v49;
		v51 = a2->getBrightness(v50, a3, v218, v215);
		v52 = this->m_pLevelSource;
		this->field_38 = v51;
		v53 = a2->getBrightness(v52, v214, v218, a5);
		v54 = this->field_B4;
		this->field_40 = v53;
		if (v54 || this->field_B6)
			this->field_28 = a2->getBrightness(this->m_pLevelSource, a3 - 1, v218, v217);
		else
			this->field_28 = this->field_2C;
		if (this->field_B7 || this->field_B6)
			this->field_30 = a2->getBrightness(this->m_pLevelSource, a3 - 1, v218, v215);
		else
			this->field_30 = this->field_2C;
		if (this->field_B4 || this->field_B5)
			this->field_3C = a2->getBrightness(this->m_pLevelSource, v214, v218, v217);
		else
			this->field_3C = this->field_40;
		if (this->field_B7 || this->field_B5)
		{
			v203 = a2->getBrightness(this->m_pLevelSource, v214, v218, v215);
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
	v65 = a6 * 0.5f;
	v66 = a7 * 0.5f;
	this->m_vtxRed[3] = a6 * 0.5f;
	this->m_vtxRed[2] = a6 * 0.5f;
	this->m_vtxRed[1] = a6 * 0.5f;
	this->m_vtxRed[0] = a6 * 0.5f;
	v67 = a8 * 0.5f;
	this->m_vtxGreen[3] = a7 * 0.5f;
	this->m_vtxGreen[2] = a7 * 0.5f;
	this->m_vtxGreen[1] = a7 * 0.5f;
	this->m_vtxGreen[0] = a7 * 0.5f;
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
	v70 = a2->getTexture(v68, a3, a4, a5, 0);
	renderFaceUp(a2, (float)a3, (float)a4, (float)a5, v70);
LABEL_20:
	if (this->m_bDisableCulling || a2->shouldRenderFace(this->m_pLevelSource, a3, v216, a5, 1))
	{
		if (this->field_78 <= 0)
		{
			v87 = this->field_20;
			v89 = v87;
			v88 = v87;
			v85 = v87;
			goto LABEL_36;
		}
		v71 = a2->getBrightness(this->m_pLevelSource, a3 - 1, v216, a5);
		v72 = this->m_pLevelSource;
		this->field_4C = v71;
		v73 = a2->getBrightness(v72, v214, v216, a5);
		v74 = this->m_pLevelSource;
		this->field_5C = v73;
		v75 = a2->getBrightness(v74, a3, v216, v217);
		v76 = this->m_pLevelSource;
		this->field_54 = v75;
		v77 = a2->getBrightness(v76, a3, v216, v215);
		v78 = this->field_AC;
		this->field_60 = v77;
		if (v78 || this->field_AE)
		{
			v197 = a2->getBrightness(this->m_pLevelSource, a3 - 1, v216, v217);
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
				v211 = a2->getBrightness(this->m_pLevelSource, a3 - 1, v216, v215);
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
				this->m_vtxRed[0] = v85 * a6;
				this->m_vtxGreen[0] = v85 * a7;
				this->m_vtxBlue[0] = v85 * a8;
				this->m_vtxRed[1] = v88 * a6;
				this->m_vtxGreen[1] = v88 * a7;
				this->m_vtxBlue[1] = v88 * a8;
				this->m_vtxRed[2] = v89 * a6;
				this->m_vtxGreen[2] = v89 * a7;
				this->m_vtxBlue[2] = v89 * a8;
				this->m_vtxRed[3] = v87 * a6;
				this->m_vtxGreen[3] = v87 * a7;
				this->m_vtxBlue[3] = v87 * a8;
				v91 = a2->getTexture(v90, a3, a4, a5, 1);
				renderFaceDown(a2, (float)a3, (float)a4, (float)a5, v91);
				goto LABEL_37;
			}
		LABEL_141:
			v213 = a2->getBrightness(this->m_pLevelSource, v214, v216, v215);
			v79 = this->field_5C;
			v80 = v213;
			this->field_64 = v213;
			goto LABEL_32;
		}
	LABEL_125:
		this->field_58 = a2->getBrightness(this->m_pLevelSource, v214, v216, v217);
		goto LABEL_27;
	}
LABEL_37:
	if (!this->m_bDisableCulling && !a2->shouldRenderFace(this->m_pLevelSource, a3, a4, v217, 2))
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
	v92 = a2->getBrightness(this->m_pLevelSource, a3 - 1, a4, v217);
	v93 = this->m_pLevelSource;
	this->field_68 = v92;
	v94 = a2->getBrightness(v93, a3, v218, v217);
	v95 = this->m_pLevelSource;
	this->field_34 = v94;
	v96 = a2->getBrightness(v95, a3, v216, v217);
	v97 = this->m_pLevelSource;
	this->field_54 = v96;
	v98 = a2->getBrightness(v97, v214, a4, v217);
	v99 = this->field_B0;
	this->field_6C = v98;
	if (v99 || this->field_B4)
	{
		v201 = a2->getBrightness(this->m_pLevelSource, a3 - 1, v218, v217);
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
		this->field_48 = a2->getBrightness(this->m_pLevelSource, a3 - 1, v216, v217);
		goto LABEL_44;
	}
	this->field_48 = this->field_68;
LABEL_44:
	if (this->field_B3 || this->field_B4)
	{
		v204 = a2->getBrightness(this->m_pLevelSource, v214, v218, v217);
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
	v206 = a2->getBrightness(this->m_pLevelSource, v214, v216, v217);
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
	v113 = a6 * 0.8f;
	v112 = a7 * 0.8f;
	this->m_vtxRed[3] = a6 * 0.8f;
	this->m_vtxRed[2] = a6 * 0.8f;
	this->m_vtxRed[1] = a6 * 0.8f;
	this->m_vtxRed[0] = a6 * 0.8f;
	v114 = a8 * 0.8f;
	this->m_vtxGreen[3] = a7 * 0.8f;
	this->m_vtxGreen[2] = a7 * 0.8f;
	this->m_vtxGreen[1] = a7 * 0.8f;
	this->m_vtxGreen[0] = a7 * 0.8f;
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
	v116 = a2->getTexture(v115, a3, a4, a5, 2);
	renderNorth(a2, (float)a3, (float)a4, (float)a5, v116);
	if (this->m_bDisableCulling)
		goto LABEL_54;
LABEL_107:
	if (!a2->shouldRenderFace(this->m_pLevelSource, a3, a4, v215, 3))
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
	v117 = a2->getBrightness(this->m_pLevelSource, a3 - 1, a4, v215);
	v118 = this->m_pLevelSource;
	this->field_70 = v117;
	v119 = a2->getBrightness(v118, v214, a4, v215);
	v120 = this->m_pLevelSource;
	this->field_74 = v119;
	v121 = a2->getBrightness(v120, a3, v218, v215);
	v122 = this->m_pLevelSource;
	this->field_38 = v121;
	v123 = a2->getBrightness(v122, a3, v216, v215);
	v124 = this->field_B2;
	this->field_60 = v123;
	if (v124 || this->field_B7)
	{
		v199 = a2->getBrightness(this->m_pLevelSource, a3 - 1, v218, v215);
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
		this->field_50 = a2->getBrightness(this->m_pLevelSource, a3 - 1, v216, v215);
		goto LABEL_60;
	}
	this->field_50 = this->field_70;
LABEL_60:
	if (this->field_B1 || this->field_B7)
	{
		v208 = a2->getBrightness(this->m_pLevelSource, v214, v218, v215);
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
	v210 = a2->getBrightness(this->m_pLevelSource, v214, v216, v215);
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
	v136 = a6 * 0.8f;
	v135 = a7 * 0.8f;
	this->m_vtxRed[3] = a6 * 0.8f;
	this->m_vtxRed[2] = a6 * 0.8f;
	this->m_vtxRed[1] = a6 * 0.8f;
	this->m_vtxRed[0] = a6 * 0.8f;
	v137 = a8 * 0.8f;
	this->m_vtxGreen[3] = a7 * 0.8f;
	this->m_vtxGreen[2] = a7 * 0.8f;
	this->m_vtxGreen[1] = a7 * 0.8f;
	this->m_vtxGreen[0] = a7 * 0.8f;
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
	v139 = a2->getTexture(v138, a3, a4, a5, 3);
	renderSouth(a2, (float)a3, (float)a4, (float)a5, v139);
	if (this->m_bDisableCulling)
		goto LABEL_70;
LABEL_109:
	if (!a2->shouldRenderFace(this->m_pLevelSource, a3 - 1, a4, a5, 4))
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
		v140 = a2->getBrightness(this->m_pLevelSource, a3 - 1, v218, a5);
		v141 = this->m_pLevelSource;
		this->field_2C = v140;
		v142 = a2->getBrightness(v141, a3 - 1, a4, v217);
		v143 = this->m_pLevelSource;
		this->field_68 = v142;
		v144 = a2->getBrightness(v143, a3 - 1, a4, v215);
		v145 = this->m_pLevelSource;
		this->field_70 = v144;
		v146 = a2->getBrightness(v145, a3 - 1, v216, a5);
		v147 = this->field_B0;
		this->field_4C = v146;
		if (v147 || this->field_B6)
			this->field_28 = a2->getBrightness(this->m_pLevelSource, a3 - 1, v218, v217);
		else
			this->field_28 = this->field_68;
		if (this->field_B2 || this->field_B6)
			this->field_30 = a2->getBrightness(this->m_pLevelSource, a3 - 1, v218, v215);
		else
			this->field_30 = this->field_70;
		if (this->field_B0 || this->field_AE)
			this->field_48 = a2->getBrightness(this->m_pLevelSource, a3 - 1, v216, v217);
		else
			this->field_48 = this->field_68;
		if (this->field_B2 || this->field_AE)
		{
			v207 = a2->getBrightness(this->m_pLevelSource, a3 - 1, v216, v215);
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
	v161 = a6 * 0.6f;
	v160 = a7 * 0.6f;
	this->m_vtxRed[3] = a6 * 0.6f;
	this->m_vtxRed[2] = a6 * 0.6f;
	this->m_vtxRed[1] = a6 * 0.6f;
	this->m_vtxRed[0] = a6 * 0.6f;
	v162 = a8 * 0.6f;
	this->m_vtxGreen[3] = a7 * 0.6f;
	this->m_vtxGreen[2] = a7 * 0.6f;
	this->m_vtxGreen[1] = a7 * 0.6f;
	this->m_vtxGreen[0] = a7 * 0.6f;
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
	v164 = a2->getTexture(v163, a3, a4, a5, 4);
	renderWest(a2, (float)a3, (float)a4, (float)a5, v164);
	if (this->m_bDisableCulling)
		goto LABEL_88;
LABEL_111:
	if (!a2->shouldRenderFace(this->m_pLevelSource, v214, a4, a5, 5))
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
	v165 = a2->getBrightness(this->m_pLevelSource, v214, v218, a5);
	v166 = this->m_pLevelSource;
	this->field_40 = v165;
	v167 = a2->getBrightness(v166, v214, a4, v217);
	v168 = this->m_pLevelSource;
	this->field_6C = v167;
	v169 = a2->getBrightness(v168, v214, a4, v215);
	v170 = this->m_pLevelSource;
	this->field_74 = v169;
	v171 = a2->getBrightness(v170, v214, v216, a5);
	v172 = this->field_B5;
	this->field_5C = v171;
	if (v172 || this->field_B3)
	{
		v173 = a2->getBrightness(this->m_pLevelSource, v214, v218, v217);
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
	this->field_44 = a2->getBrightness(this->m_pLevelSource, v214, v218, v215);
LABEL_94:
	if (this->field_AD || this->field_B3)
	{
		v175 = a2->getBrightness(this->m_pLevelSource, v214, v216, v217);
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
			v177 = a2->getBrightness(this->m_pLevelSource, v214, v216, v215);
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
	v189 = a6 * 0.6f;
	v190 = a7 * 0.6f;
	this->m_vtxRed[3] = a6 * 0.6f;
	this->m_vtxRed[2] = a6 * 0.6f;
	this->m_vtxRed[1] = a6 * 0.6f;
	this->m_vtxRed[0] = a6 * 0.6f;
	v191 = a8 * 0.6f;
	this->m_vtxGreen[3] = a7 * 0.6f;
	this->m_vtxGreen[2] = a7 * 0.6f;
	this->m_vtxGreen[1] = a7 * 0.6f;
	this->m_vtxGreen[0] = a7 * 0.6f;
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
	v193 = a2->getTexture(v192, a3, a4, a5, 5);
	renderEast(a2, (float)a3, (float)a4, (float)a5, v193);
LABEL_102:
	result = v69;
	this->m_bAmbientOcclusion = 0;
	return result;
}

#ifdef ENH_SHADE_HELD_TILES

#define SHADE_PREPARE \
	float red = bright, grn = bright, blu = bright; \
	if (tile->m_ID == Tile::leaves->m_ID)           \
		red *= 0.35f, grn *= 0.65f, blu *= 0.25f;

#define SHADE_IF_NEEDED(col) t.color(col*red,col*grn,col*blu,1.0f)

#else

#define SHADE_PREPARE ;
#define SHADE_IF_NEEDED(col) ;

#endif

void TileRenderer::renderTile(Tile* tile, int data RENDER_TILE_ARG_PATCH)
{
	Tesselator& t = Tesselator::instance;

#ifdef ENH_SHADE_HELD_TILES
	t.color(bright, bright, bright);
#else
	t.color(255, 255, 255);
#endif

	int shape = tile->getRenderShape();
	if (shape == SHAPE_SOLID)
	{
		glTranslatef(-0.5f, -0.5f, -0.5f);
		t.begin();
		SHADE_PREPARE;
		SHADE_IF_NEEDED(0.5f);
		renderFaceUp  (tile, 0.0f, 0.0f, 0.0f, tile->getTexture(DIR_YNEG, data));
		SHADE_IF_NEEDED(1.0f);
		renderFaceDown(tile, 0.0f, 0.0f, 0.0f, tile->getTexture(DIR_YPOS, data));
		SHADE_IF_NEEDED(0.6f);
		renderNorth   (tile, 0.0f, 0.0f, 0.0f, tile->getTexture(DIR_ZNEG, data));
		renderSouth   (tile, 0.0f, 0.0f, 0.0f, tile->getTexture(DIR_ZPOS, data));
		SHADE_IF_NEEDED(0.8f);
		renderWest    (tile, 0.0f, 0.0f, 0.0f, tile->getTexture(DIR_XNEG, data));
		renderEast    (tile, 0.0f, 0.0f, 0.0f, tile->getTexture(DIR_XPOS, data));
		SHADE_IF_NEEDED(1.0f);
		t.draw();
		glTranslatef(0.5f, 0.5f, 0.5f);
		return;
	}
	if (shape == SHAPE_CROSS)
	{
		t.begin();
		tesselateCrossTexture(tile, data, -0.5f, -0.5f, -0.5f);
		t.draw();
		return;
	}
	if (shape == SHAPE_STAIRS)
	{
#ifdef ORIGINAL_CODE
		for (int i = 0; i < 2; i++)
		{
			if (i)
				tile->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f);
			else
				tile->setShape(0.0f, 0.0f, 0.5f, 1.0f, 0.5f, 1.0f);

			
			glTranslatef(-0.5f, -0.5f, -0.5f);

			// @BUG: Beginning twice and never drawing
			// @BUG: Using glTranslatef when it only affects the final thing you're drawing
			t.begin();

			renderFaceUp  (tile, 0.0f, 0.0f, 0.0f, tile->getTexture(DIR_YNEG, data));
			renderFaceDown(tile, 0.0f, 0.0f, 0.0f, tile->getTexture(DIR_YPOS, data));
			renderNorth   (tile, 0.0f, 0.0f, 0.0f, tile->getTexture(DIR_ZNEG, data));
			renderSouth   (tile, 0.0f, 0.0f, 0.0f, tile->getTexture(DIR_ZPOS, data));
			renderWest    (tile, 0.0f, 0.0f, 0.0f, tile->getTexture(DIR_XNEG, data));
			renderEast    (tile, 0.0f, 0.0f, 0.0f, tile->getTexture(DIR_XPOS, data));
			glTranslatef(0.5f, 0.5f, 0.5f);
		}
#else
		// Fixed version from 0.1.1j+
		t.addOffset(-0.5f, -0.5f, -0.5f);
		for (int i = 0; i < 2; i++)
		{
			if (i)
				tile->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f);
			else
				tile->setShape(0.0f, 0.0f, 0.5f, 1.0f, 0.5f, 1.0f);


			t.begin();
			SHADE_PREPARE;
			SHADE_IF_NEEDED(0.5f);
			renderFaceUp  (tile, 0.0f, 0.0f, 0.0f, tile->getTexture(DIR_YNEG, data));
			SHADE_IF_NEEDED(1.0f);
			renderFaceDown(tile, 0.0f, 0.0f, 0.0f, tile->getTexture(DIR_YPOS, data));
			SHADE_IF_NEEDED(0.6f);
			renderNorth   (tile, 0.0f, 0.0f, 0.0f, tile->getTexture(DIR_ZNEG, data));
			renderSouth   (tile, 0.0f, 0.0f, 0.0f, tile->getTexture(DIR_ZPOS, data));
			SHADE_IF_NEEDED(0.8f);
			renderWest    (tile, 0.0f, 0.0f, 0.0f, tile->getTexture(DIR_XNEG, data));
			renderEast    (tile, 0.0f, 0.0f, 0.0f, tile->getTexture(DIR_XPOS, data));
			SHADE_IF_NEEDED(1.0f);
			t.draw();
		}
		t.addOffset(0.5f, 0.5f, 0.5f);
#endif

		return;
	}
}

#ifdef ENH_USE_OWN_AO
bool TileRenderer::tesselateBlockInWorldWithAmbienceOcclusionV2(Tile* tile, int x, int y, int z, float r, float g, float b)
{
	// START OF AUXILIARY DATA FOR AO

	// Well, this is a big enum.
	enum eDirection
	{
		EDIR_HERE,  // Here (no change in coordinates)
		EDIR_N,  // -Z
		EDIR_S,  // +Z
		EDIR_W,  // -X
		EDIR_E,  // +X
		EDIR_NW, // -X-Z
		EDIR_NE, // +X-Z
		EDIR_SW, // -X+Z
		EDIR_SE, // +X+Z

		EDIR_U,  // +Y
		EDIR_UN, // +Y-Z
		EDIR_US, // +Y+Z
		EDIR_UW, // +Y-X
		EDIR_UE, // +Y+X
		EDIR_UNW,// +Y-X-Z
		EDIR_UNE,// +Y+X-Z
		EDIR_USW,// +Y-X+Z
		EDIR_USE,// +Y+X+Z

		EDIR_D,  // -Y
		EDIR_DN, // -Y-Z
		EDIR_DS, // -Y+Z
		EDIR_DW, // -Y-X
		EDIR_DE, // -Y+X
		EDIR_DNW,// -Y-X-Z
		EDIR_DNE,// -Y+X-Z
		EDIR_DSW,// -Y-X+Z
		EDIR_DSE,// -Y+X+Z
		EDIR_COUNT,
	};

	// for DIR_? directions
	static const int diffX[] = { 0,0,0,0,-1,1 };
	static const int diffZ[] = { 0,0,-1,1,0,0 };
	static const int diffY[] = { -1,1,0,0,0,0 };

	// for EDIR_? directions
	static const int diffEX[] = { 0,0,0,-1,+1,-1,+1,-1,+1,0,0,0,-1,+1,-1,+1,-1,+1,0,0,0,-1,+1,-1,+1,-1,+1 };
	static const int diffEZ[] = { 0,-1,+1,0,0,-1,-1,+1,+1,0,-1,+1,0,0,-1,-1,+1,+1,0,-1,+1,0,0,-1,-1,+1,+1 };
	static const int diffEY[] = { 0,0,0,0,0,0,0,0,0, +1,+1,+1,+1,+1,+1,+1,+1,+1, -1,-1,-1,-1,-1,-1,-1,-1,-1 };

	// Convert a DIR_? to an EDIR_?
	static const int dirToEdir[] = { EDIR_D, EDIR_U, EDIR_N, EDIR_S, EDIR_W, EDIR_E };

	// this is a huge table. Essentially this tells us which 3 tiles we should base our lighting value on aside from the
	// one that's pointed to by the face we're rendering.
	// The array is 6 * 4 * 3. 6 for the directions, 4 for the number of vertices per face, and 3 for the number of other tiles we'll check.

	static const int massLUT[] = {
		EDIR_DS, EDIR_DW, EDIR_DSW,   EDIR_DN, EDIR_DW, EDIR_DNW,   EDIR_DN, EDIR_DE, EDIR_DNE,   EDIR_DS, EDIR_DE, EDIR_DSE,
		EDIR_US, EDIR_UE, EDIR_USE,   EDIR_UN, EDIR_UE, EDIR_UNE,   EDIR_UN, EDIR_UW, EDIR_UNW,   EDIR_US, EDIR_UW, EDIR_USW,
		EDIR_UN, EDIR_NW, EDIR_UNW,   EDIR_UN, EDIR_NE, EDIR_UNE,   EDIR_DN, EDIR_NE, EDIR_DNE,   EDIR_DN, EDIR_NW, EDIR_DNW,
		EDIR_US, EDIR_SW, EDIR_USW,   EDIR_DS, EDIR_SW, EDIR_DSW,   EDIR_DS, EDIR_SE, EDIR_DSE,   EDIR_US, EDIR_SE, EDIR_USE,
		EDIR_UW, EDIR_SW, EDIR_USW,   EDIR_UW, EDIR_NW, EDIR_UNW,   EDIR_DW, EDIR_NW, EDIR_DNW,   EDIR_DW, EDIR_SW, EDIR_DSW,
		EDIR_DE, EDIR_SE, EDIR_DSE,   EDIR_DE, EDIR_NE, EDIR_DNE,   EDIR_UE, EDIR_NE, EDIR_UNE,   EDIR_UE, EDIR_SE, EDIR_USE,
	};

	// Depending on the face, add a multiplier to our lighting value. This is the same as in regular block rendering without AO.
	static const float lightingMult[] = { 0.5f, 1.0f, 0.8f, 0.8f, 0.6f, 0.6f };

	// END OF AUXILIARY DATA FOR AO

	bool bBother = false;

	// Check if we should bother at all. Most of the time, we don't. There are some calculations that we shouldn't do unless necessary
	for (int dir = DIR_YNEG; dir <= DIR_XPOS; dir++)
	{
		int tileX = x + diffX[dir], tileY = y + diffY[dir], tileZ = z + diffZ[dir];

		if (!m_bDisableCulling && !tile->shouldRenderFace(m_pLevelSource, tileX, tileY, tileZ, dir))
			continue;

		bBother = true;
		break;
	}

	if (!bBother)
		return false;

	float topR = r, topG = g, topB = b;

	if (tile == Tile::grass)
		r = g = b = 1.0f;

	Tesselator& t = Tesselator::instance;

	float fLightHere = tile->getBrightness(m_pLevelSource, x, y, z);

	float lights[EDIR_COUNT];

	// Get the brightness of the tile we're processing, as well as all tiles around it
	lights[EDIR_HERE] = tile->getBrightness(m_pLevelSource, x, y, z);

	for (int i = 1; i < EDIR_COUNT; i++)
	{
		lights[i] = tile->getBrightness(m_pLevelSource, x + diffEX[i], y + diffEY[i], z + diffEZ[i]);
	}

	// Render all the faces.
	for (int dir = DIR_YNEG; dir <= DIR_XPOS; dir++)
	{
		int tileX = x + diffX[dir], tileY = y + diffY[dir], tileZ = z + diffZ[dir];

		// check if we should bother in the first place
		if (!m_bDisableCulling && !tile->shouldRenderFace(m_pLevelSource, tileX, tileY, tileZ, dir))
		{
			continue;
		}

		float fR = r, fG = g, fB = b;
		if (dir == DIR_YPOS)
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

		switch (dir) {
			case DIR_YNEG:
				renderFaceUp  (tile, float(x), float(y), float(z), tile->getTexture(m_pLevelSource, x, y, z, DIR_YNEG));
				break;
			case DIR_YPOS:
				renderFaceDown(tile, float(x), float(y), float(z), tile->getTexture(m_pLevelSource, x, y, z, DIR_YPOS));
				break;
			case DIR_ZNEG:
				renderNorth   (tile, float(x), float(y), float(z), tile->getTexture(m_pLevelSource, x, y, z, DIR_ZNEG));
				break;
			case DIR_ZPOS:
				renderSouth   (tile, float(x), float(y), float(z), tile->getTexture(m_pLevelSource, x, y, z, DIR_ZPOS));
				break;
			case DIR_XNEG:
				renderWest    (tile, float(x), float(y), float(z), tile->getTexture(m_pLevelSource, x, y, z, DIR_XNEG));
				break;
			case DIR_XPOS:
				renderEast    (tile, float(x), float(y), float(z), tile->getTexture(m_pLevelSource, x, y, z, DIR_XPOS));
				break;
		}

		m_bAmbientOcclusion = false;
	}

	return true;
}
#endif
