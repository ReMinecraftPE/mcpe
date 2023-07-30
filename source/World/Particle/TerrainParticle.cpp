/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Particle.hpp"
#include "Level.hpp"

TerrainParticle::TerrainParticle(Level* level, float x, float y, float z, Tile* tile) :
	TerrainParticle(level, x, y, z, 0.0f, 0.0f, 0.0f, tile)
{
}

TerrainParticle::TerrainParticle(Level* level, float x, float y, float z, float vx, float vy, float vz, Tile* tile) :
	Particle(level, x, y, z, vx, vy, vz)
{
	m_pTile = tile;
	field_DC = tile->m_TextureFrame;
	field_F4 = tile->field_28;
	field_100 = 0.6f;
	field_FC = 0.6f;
	field_F8 = 0.6f;
	field_F0 *= 0.5f;
}

TerrainParticle* TerrainParticle::init(int x, int y, int z)
{
	if (m_pTile == Tile::grass)
		return this;

	int color = m_pTile->getColor(m_pLevel, x, y, z);
	field_F8 *= float(GET_RED(color)) / 255.0f;
	field_FC *= float(GET_GREEN(color)) / 255.0f;
	field_100 *= float(GET_BLUE(color)) / 255.0f;

	return this;
}

int TerrainParticle::getParticleTexture()
{
	return PT_TERRAIN;
}

void TerrainParticle::render(Tesselator& t, float f, float a4, float a5, float a6, float a7, float a8)
{
	constexpr float C_MAGIC_1 = 0.015609f; // @BUG: Slightly bigger than 1/64.0f

	int texture = field_DC;
	int texX = texture % 16;
	if (texture < 0)
		texture += 15;

	float texU_1 = (float(texX)         + 0.25f * field_E0) / 16.0f;
	float texV_1 = (float(texture >> 4) + 0.25f * field_E4) / 16.0f;

	float posX = Lerp(field_3C.x, m_pos.x, f) - xOff;
	float posY = Lerp(field_3C.y, m_pos.y, f) - yOff;
	float posZ = Lerp(field_3C.z, m_pos.z, f) - zOff;
	float fBright = getBrightness(f);

	float sizeX = a4 * field_F0 * 0.1f;
	float sizeY = a5 * field_F0 * 0.1f;
	float sizeZ = a6 * field_F0 * 0.1f;
	float siz2X = a7 * field_F0 * 0.1f;
	float siz2Z = a8 * field_F0 * 0.1f;

	t.color(field_F8 * fBright, field_FC * fBright, field_100 * fBright);
	t.vertexUV(posX - sizeX - siz2X, posY - sizeY, posZ - sizeZ - siz2Z, texU_1 + C_MAGIC_1, texV_1 + C_MAGIC_1);
	t.vertexUV(posX - sizeX + siz2X, posY + sizeY, posZ - sizeZ + siz2Z, texU_1 + C_MAGIC_1, texV_1);
	t.vertexUV(posX + sizeX + siz2X, posY + sizeY, posZ + sizeZ + siz2Z, texU_1, texV_1);
	t.vertexUV(posX + sizeX - siz2X, posY - sizeY, posZ + sizeZ - siz2Z, texU_1, texV_1 + C_MAGIC_1);
}