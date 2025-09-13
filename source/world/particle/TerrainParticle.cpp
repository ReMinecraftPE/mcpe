/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Particle.hpp"
#include "world/level/Level.hpp"

void TerrainParticle::_init(Tile* tile)
{
	m_pTile = tile;
	field_DC = tile->m_TextureFrame;
	field_F4 = tile->field_28;
	m_rCol = m_gCol = m_bCol = 0.6f;
	field_F0 *= 0.5f;
}

TerrainParticle::TerrainParticle(Level* level, const Vec3& pos, Tile* tile) :
	Particle(level, pos, Vec3::ZERO)
{
	_init(tile);
}

TerrainParticle::TerrainParticle(Level* level, const Vec3& pos, const Vec3& dir, Tile* tile) :
	Particle(level, pos, dir)
{
	_init(tile);
}

TerrainParticle* TerrainParticle::init(const TilePos& tilePos, Facing::Name face)
{
#ifndef ENH_FACED_TERRAIN_PARTICLES
	face = Facing::DOWN;
#endif

	field_DC = m_pTile->getTexture(m_pLevel, tilePos, face);

	if (m_pTile == Tile::grass && face != Facing::UP)
		return this;

	int color = m_pTile->getColor(m_pLevel, tilePos);
	m_rCol *= float(GET_RED(color)) / 255.0f;
	m_gCol *= float(GET_GREEN(color)) / 255.0f;
	m_bCol *= float(GET_BLUE(color)) / 255.0f;

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

	float posX = Mth::Lerp(m_oPos.x, m_pos.x, f) - xOff;
	float posY = Mth::Lerp(m_oPos.y, m_pos.y, f) - yOff;
	float posZ = Mth::Lerp(m_oPos.z, m_pos.z, f) - zOff;
	float fBright = getBrightness(f);

	float sizeX = a4 * field_F0 * 0.1f;
	float sizeY = a5 * field_F0 * 0.1f;
	float sizeZ = a6 * field_F0 * 0.1f;
	float siz2X = a7 * field_F0 * 0.1f;
	float siz2Z = a8 * field_F0 * 0.1f;

	t.color(m_rCol * fBright, m_gCol * fBright, m_bCol * fBright);
	t.vertexUV(posX - sizeX - siz2X, posY - sizeY, posZ - sizeZ - siz2Z, texU_1 + C_MAGIC_1, texV_1 + C_MAGIC_1);
	t.vertexUV(posX - sizeX + siz2X, posY + sizeY, posZ - sizeZ + siz2Z, texU_1 + C_MAGIC_1, texV_1);
	t.vertexUV(posX + sizeX + siz2X, posY + sizeY, posZ + sizeZ + siz2Z, texU_1, texV_1);
	t.vertexUV(posX + sizeX - siz2X, posY - sizeY, posZ + sizeZ - siz2Z, texU_1, texV_1 + C_MAGIC_1);
}