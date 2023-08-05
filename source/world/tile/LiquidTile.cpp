/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Tile.hpp"
#include "world/level/Level.hpp"

LiquidTile::LiquidTile(int id, Material* pMtl) : Tile(id, pMtl == Material::lava ? TEXTURE_LAVA : TEXTURE_WATER, pMtl)
{
	setTicking(true);
}

void LiquidTile::animateTick(Level* level, int x, int y, int z, Random* random)
{
	if (m_pMaterial == Material::water)
	{
		if (!random->nextInt(64))
			// @BUG: Return value unused.
			level->getData(x, y, z);
	}

	// @BUG: Redundant check for isSolidTile?
	if (m_pMaterial == Material::lava && level->getMaterial(x, y + 1, z) == Material::air && !level->isSolidTile(x, y + 1, z) && !random->nextInt(3))
	{
		level->addParticle("lava", x + random->nextFloat(), y + m_aabb.max.y, z + random->nextFloat(), 0.0f, 0.0f, 0.0f);
	}
}

void LiquidTile::fizz(Level* level, int x, int y, int z)
{
	for (int i = 0; i < 8; i++)
	{
		level->addParticle("largesmoke", x + Mth::random(), y + 1.2f, z + Mth::random(), 0.0f, 0.0f, 0.0f);
	}
}

AABB* LiquidTile::getAABB(Level*, int x, int y, int z)
{
	return nullptr;
}

float LiquidTile::getBrightness(LevelSource* level, int x, int y, int z)
{
	float b1 = level->getBrightness(x, y, z);
	float b2 = level->getBrightness(x, y + 1, z);
	if (b1 <= b2)
		b1 = b2;
	return b1;
}

int LiquidTile::getColor(LevelSource* level, int x, int y, int z)
{
	return 0x999999FF;
}

int LiquidTile::getDepth(Level* level, int x, int y, int z)
{
	if (level->getMaterial(x, y, z) != m_pMaterial)
		return -1;

	return level->getData(x, y, z);
}

int LiquidTile::getRenderedDepth(LevelSource* level, int x, int y, int z)
{
	if (level->getMaterial(x, y, z) != m_pMaterial)
		return -1;

	int res = level->getData(x, y, z);
	if (res > 7)
		res = 0;

	return res;
}

Vec3 LiquidTile::getFlow(LevelSource* level, int x, int y, int z)
{
	Vec3 result;
	int depthLocal = getRenderedDepth(level, x, y, z);
	for (int i = 0; i < 4; i++)
	{
		int checkX = x, checkY = y, checkZ = z;
		switch (i)
		{
			case 0: checkX--; break;
			case 1: checkZ--; break;
			case 2: checkX++; break;
			case 3: checkZ++; break;
		}

		int depthCheck = getRenderedDepth(level, checkX, checkY, checkZ);
		if (depthCheck < 0)
		{
			if (level->getMaterial(checkX, checkY, checkZ)->blocksMotion())
				continue;

			depthCheck = getRenderedDepth(level, checkX, checkY - 1, checkZ);
			if (depthCheck >= 0)
			{
				int mult = depthCheck - (depthLocal - 8);
				result += Vec3(float((checkX - x) * mult), float((checkY - y) * mult), float((checkZ - z) * mult));
			}
			continue;
		}
		else
		{
			int mult = depthCheck - depthLocal;
			result += Vec3(float((checkX - x) * mult), float((checkY - y) * mult), float((checkZ - z) * mult));
		}
	}

	if (level->getData(x, y, z) >= 8)
	{
		if (shouldRenderFace(level, x, y, z - 1, DIR_ZNEG) ||
			shouldRenderFace(level, x, y, z + 1, DIR_ZPOS) ||
			shouldRenderFace(level, x - 1, y, z, DIR_XNEG) ||
			shouldRenderFace(level, x + 1, y, z, DIR_XPOS) ||
			shouldRenderFace(level, x, y + 1, z - 1, DIR_ZNEG) ||
			shouldRenderFace(level, x, y + 1, z + 1, DIR_ZPOS) ||
			shouldRenderFace(level, x - 1, y + 1, z, DIR_XNEG) ||
			shouldRenderFace(level, x + 1, y + 1, z, DIR_XPOS))
		{
			result = result.normalize() + Vec3(0, -6, 0);
		}
	}

	return result.normalize();
}

int LiquidTile::getRenderLayer()
{
	return m_pMaterial == Material::water ? LAYER_ALPHA : LAYER_OPAQUE;
}

int LiquidTile::getRenderShape()
{
	return SHAPE_WATER;
}

int LiquidTile::getResource(int x, Random* random)
{
	return 0;
}

int LiquidTile::getResourceCount(Random* random)
{
	return 0;
}

float LiquidTile::getSlopeAngle(LevelSource* level, int x, int y, int z, Material* pMtl)
{
	Vec3 vec;
	if (pMtl == Material::water)
		vec = ((LiquidTile*)Tile::water)->getFlow(level, x, y, z);
	if (pMtl == Material::lava)
		vec = ((LiquidTile*)Tile::lava)->getFlow(level, x, y, z);

	if (vec.x == 0 && vec.z == 0)
		return -1000.0f;

	return atan2f(vec.z, vec.x) + float(-0.5f * 3.1416f);
}

int LiquidTile::getTexture(int dir)
{
	if (dir > 1)
		return m_TextureFrame + 1;

	return m_TextureFrame;
}

int LiquidTile::getTexture(int dir, int data)
{
	// @TODO: revert to using Tile::getTexture
	return Tile::getTexture(dir, data);
}

int LiquidTile::getTickDelay()
{
	if (m_pMaterial == Material::water)
		return 5;
	if (m_pMaterial == Material::lava)
		return 30;

	return 0;
}

void LiquidTile::handleEntityInside(Level* level, int x, int y, int z, Entity* pEnt, Vec3& vec)
{
	vec += getFlow(level, x, y, z);
}

bool LiquidTile::isCubeShaped()
{
	return false;
}

bool LiquidTile::isSolidRender()
{
	return false;
}

bool LiquidTile::mayPick(int data, bool b)
{
	if (!b)
		return false;

	return data == 0;
}

void LiquidTile::neighborChanged(Level* level, int x, int y, int z, int dir)
{
	updateLiquid(level, x, y, z);
}

void LiquidTile::onPlace(Level* level, int x, int y, int z)
{
	updateLiquid(level, x, y, z);
}

bool LiquidTile::shouldRenderFace(LevelSource* level, int x, int y, int z, int dir)
{
	Material* pMtl = level->getMaterial(x, y, z);
	if (pMtl == m_pMaterial || pMtl == Material::ice)
		return false;

	if (dir == DIR_YPOS)
		return true;

	return Tile::shouldRenderFace(level, x, y, z, dir);
}

void LiquidTile::tick(Level*, int x, int y, int z, Random* random)
{
}

void LiquidTile::updateLiquid(Level* level, int x, int y, int z)
{
	if (level->getTile(x, y, z) != m_ID)
		return;

	if (m_pMaterial != Material::lava)
		// such interactions do not apply to water
		return;

	if (level->getMaterial(x, y, z - 1) == Material::water ||
		level->getMaterial(x, y, z + 1) == Material::water ||
		level->getMaterial(x - 1, y, z) == Material::water ||
		level->getMaterial(x + 1, y, z) == Material::water ||
		level->getMaterial(x, y + 1, z) == Material::water)
	{
		Tile* newTile;
		int data = level->getData(x, y, z);

		if (data == 0)
		{
			newTile = Tile::obsidian;
		}
		else if (data > 4)
		{
			// @NOTE: huh?
			fizz(level, x, y, z);
			return;
		}
		else
		{
			newTile = Tile::stoneBrick;
		}

		level->setTile(x, y, z, newTile->m_ID);

		fizz(level, x, y, z);
	}
}
