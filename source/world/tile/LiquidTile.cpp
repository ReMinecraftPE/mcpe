/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LiquidTile.hpp"
#include "world/level/Level.hpp"

LiquidTile::LiquidTile(int id, Material* pMtl) : Tile(id, pMtl == Material::lava ? TEXTURE_LAVA : TEXTURE_WATER, pMtl)
{
	field_6C = 0;
	field_70[0] = 0;
	field_74[0] = 0;

	setTicking(true);
}

void LiquidTile::animateTick(Level* level, const TilePos& pos, Random* random)
{
	if (m_pMaterial == Material::water)
	{
		if (!random->nextInt(64))
			// @BUG: Return value unused.
			level->getData(pos);
	}

	// @BUG: Redundant check for isSolidTile?
	if (m_pMaterial == Material::lava && level->getMaterial(pos.above()) == Material::air && !level->isSolidTile(pos.above()) && !random->nextInt(3))
	{
		level->addParticle("lava", Vec3(pos.x + random->nextFloat(), pos.y + m_aabb.max.y, pos.z + random->nextFloat()));
	}
}

void LiquidTile::fizz(Level* level, const TilePos& pos)
{
	for (int i = 0; i < 8; i++)
	{
		level->addParticle("largesmoke", Vec3(pos.x + Mth::random(), pos.y + 1.2f, pos.z + Mth::random()));
	}
}

AABB* LiquidTile::getAABB(const Level*, const TilePos& pos)
{
	return nullptr;
}

float LiquidTile::getBrightness(const LevelSource* level, const TilePos& pos) const
{
	float b1 = level->getBrightness(pos);
	float b2 = level->getBrightness(pos.above());
	if (b1 <= b2)
		b1 = b2;
	return b1;
}

int LiquidTile::getColor(const LevelSource* level, const TilePos& pos) const
{
	return 0x999999FF;
}

int LiquidTile::getDepth(Level* level, const TilePos& pos)
{
	if (level->getMaterial(pos) != m_pMaterial)
		return -1;

	return level->getData(pos);
}

int LiquidTile::getRenderedDepth(const LevelSource* level, const TilePos& pos) const
{
	if (level->getMaterial(pos) != m_pMaterial)
		return -1;

	int res = level->getData(pos);
	if (res > 7)
		res = 0;

	return res;
}

Vec3 LiquidTile::getFlow(const LevelSource* level, const TilePos& pos) const
{
	Vec3 result;
	int depthLocal = getRenderedDepth(level, pos);
	for (int i = 0; i < 4; i++)
	{
		TilePos check(pos);
		switch (i)
		{
			case 0: check.x--; break;
			case 1: check.z--; break;
			case 2: check.x++; break;
			case 3: check.z++; break;
		}

		int depthCheck = getRenderedDepth(level, check);
		if (depthCheck < 0)
		{
			if (level->getMaterial(check)->blocksMotion())
				continue;

			depthCheck = getRenderedDepth(level, TilePos(check.x, check.y - 1, check.z));
			if (depthCheck >= 0)
			{
				int mult = depthCheck - (depthLocal - 8);
				// Not sure what the difference is between these two, but the top one breaks flow
				//result += Vec3((check - pos) * mult);
				result += Vec3(float((check.x - pos.x) * mult), float((check.y - pos.y) * mult), float((check.z - pos.z) * mult));
			}
			continue;
		}
		else
		{
			int mult = depthCheck - depthLocal;
			//result += Vec3((check - pos) * mult);
			result += Vec3(float((check.x - pos.x) * mult), float((check.y - pos.y) * mult), float((check.z - pos.z) * mult));
		}
	}

	if (level->getData(pos) >= 8)
	{
		if (shouldRenderFace(level, pos.north(), Facing::NORTH) ||
			shouldRenderFace(level, pos.south(), Facing::SOUTH) ||
			shouldRenderFace(level, pos.west(), Facing::WEST) ||
			shouldRenderFace(level, pos.east(), Facing::EAST) ||
			shouldRenderFace(level, pos.above().north(), Facing::NORTH) ||
			shouldRenderFace(level, pos.above().south(), Facing::SOUTH) ||
			shouldRenderFace(level, pos.above().west(), Facing::WEST) ||
			shouldRenderFace(level, pos.above().east(), Facing::EAST))
		{
			result = result.normalize() + Vec3(0, -6, 0);
		}
	}

	return result.normalize();
}

int LiquidTile::getRenderLayer() const
{
	return m_pMaterial == Material::water ? LAYER_ALPHA : LAYER_OPAQUE;
}

int LiquidTile::getRenderShape() const
{
	return SHAPE_WATER;
}

int LiquidTile::getResource(TileData data, Random* random) const
{
	return 0;
}

int LiquidTile::getResourceCount(Random* random) const
{
	return 0;
}

float LiquidTile::getSlopeAngle(const LevelSource* level, const TilePos& pos, const Material* pMtl)
{
	Vec3 vec;
	if (pMtl == Material::water)
		vec = ((LiquidTile*)Tile::water)->getFlow(level, pos);
	if (pMtl == Material::lava)
		vec = ((LiquidTile*)Tile::lava)->getFlow(level, pos);

	if (vec.x == 0 && vec.z == 0)
		return -1000.0f;

	return atan2f(vec.z, vec.x) + float(-0.5f * 3.1416f);
}

int LiquidTile::getTexture(Facing::Name face) const
{
	if (face > Facing::UP)
		return m_TextureFrame + 1;

	return m_TextureFrame;
}

int LiquidTile::getTexture(Facing::Name face, TileData data) const
{
	// @TODO: revert to using Tile::getTexture
	return Tile::getTexture(face, data);
}

int LiquidTile::getTickDelay() const
{
	if (m_pMaterial == Material::water)
		return 5;
	if (m_pMaterial == Material::lava)
		return 30;

	return 0;
}

void LiquidTile::handleEntityInside(Level* level, const TilePos& pos, const Entity* pEnt, Vec3& vec)
{
	vec += getFlow(level, pos);
}

bool LiquidTile::isCubeShaped() const
{
	return false;
}

bool LiquidTile::isSolidRender() const
{
	return false;
}

bool LiquidTile::mayPick(TileData data, bool b) const
{
	if (!b)
		return false;

	return data == 0;
}

void LiquidTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	updateLiquid(level, pos);
}

void LiquidTile::onPlace(Level* level, const TilePos& pos)
{
	updateLiquid(level, pos);
}

bool LiquidTile::shouldRenderFace(const LevelSource* level, const TilePos& pos, Facing::Name face) const
{
	Material* pMtl = level->getMaterial(pos);
	if (pMtl == m_pMaterial || pMtl == Material::ice)
		return false;

	if (face == Facing::UP)
		return true;

	return Tile::shouldRenderFace(level, pos, face);
}

void LiquidTile::tick(Level*, const TilePos& pos, Random* random)
{
}

void LiquidTile::updateLiquid(Level* level, const TilePos& pos)
{
	if (level->getTile(pos) != m_ID)
		return;

	if (m_pMaterial != Material::lava)
		// such interactions do not apply to water
		return;

	if (level->getMaterial(pos.north()) == Material::water ||
		level->getMaterial(pos.south()) == Material::water ||
		level->getMaterial(pos.west()) == Material::water ||
		level->getMaterial(pos.east()) == Material::water ||
		level->getMaterial(pos.above()) == Material::water)
	{
		Tile* newTile;
		TileData data = level->getData(pos);

		if (data == 0)
		{
			newTile = Tile::obsidian;
		}
		else if (data > 4)
		{
			// @NOTE: huh?
			fizz(level, pos);
			return;
		}
		else
		{
			newTile = Tile::stoneBrick;
		}

		level->setTile(pos, newTile->m_ID);

		fizz(level, pos);
	}
}
