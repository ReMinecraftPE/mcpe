/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LiquidTile.hpp"
#include "world/level/Level.hpp"
#include "world/level/TileSource.hpp"

LiquidTile::LiquidTile(int id, Material* pMtl) : Tile(id, pMtl == Material::lava ? TEXTURE_LAVA : TEXTURE_WATER, pMtl)
{
	m_renderLayer = m_pMaterial == Material::water ? RENDER_LAYER_BLEND : RENDER_LAYER_OPAQUE;
	field_6C = 0;
	field_70[0] = 0;
	field_74[0] = 0;

	setTicking(true);
}

void LiquidTile::animateTick(TileSource* source, const TilePos& pos, Random* random)
{
	if (m_pMaterial == Material::water)
	{
		if (!random->nextInt(64))
			// @BUG: Return value unused.
			source->getData(pos);
	}

	// @BUG: Redundant check for isSolidTile?
	if (m_pMaterial == Material::lava && source->getMaterial(pos.above()) == Material::air && !source->isSolidBlockingTile(pos.above()) && !random->nextInt(3))
	{
		source->getLevel().addParticle("lava", Vec3(pos.x + random->nextFloat(), pos.y + m_aabb.max.y, pos.z + random->nextFloat()));
	}
}

void LiquidTile::fizz(TileSource* source, const TilePos& pos)
{
	for (int i = 0; i < 8; i++)
	{
		source->getLevel().addParticle("largesmoke", Vec3(pos.x + Mth::random(), pos.y + 1.2f, pos.z + Mth::random()));
	}
}

AABB* LiquidTile::getAABB(TileSource* source, const TilePos& pos)
{
	return nullptr;
}

float LiquidTile::getBrightness(TileSource* source, const TilePos& pos) const
{
	float b1 = source->getBrightness(pos);
	float b2 = source->getBrightness(pos.above());
	if (b1 <= b2)
		b1 = b2;
	return b1;
}

int LiquidTile::getColor(TileSource* source, const TilePos& pos) const
{
	return 0x999999FF;
}

int LiquidTile::getDepth(TileSource* source, const TilePos& pos)
{
	if (source->getMaterial(pos) != m_pMaterial)
		return -1;

	return source->getData(pos);
}

int LiquidTile::getRenderedDepth(TileSource* source, const TilePos& pos) const
{
	if (source->getMaterial(pos) != m_pMaterial)
		return -1;

	int res = source->getData(pos);
	if (res > 7)
		res = 0;

	return res;
}

Vec3 LiquidTile::getFlow(TileSource* source, const TilePos& pos) const
{
	Vec3 result;
	int depthLocal = getRenderedDepth(source, pos);
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

		int depthCheck = getRenderedDepth(source, check);
		if (depthCheck < 0)
		{
			if (source->getMaterial(check)->blocksMotion())
				continue;

			depthCheck = getRenderedDepth(source, TilePos(check.x, check.y - 1, check.z));
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

	if (source->getData(pos) >= 8)
	{
		if (shouldRenderFace(source, pos.north(), Facing::NORTH) ||
			shouldRenderFace(source, pos.south(), Facing::SOUTH) ||
			shouldRenderFace(source, pos.west(), Facing::WEST) ||
			shouldRenderFace(source, pos.east(), Facing::EAST) ||
			shouldRenderFace(source, pos.above().north(), Facing::NORTH) ||
			shouldRenderFace(source, pos.above().south(), Facing::SOUTH) ||
			shouldRenderFace(source, pos.above().west(), Facing::WEST) ||
			shouldRenderFace(source, pos.above().east(), Facing::EAST))
		{
			result = result.normalize() + Vec3(0, -6, 0);
		}
	}

	return result.normalize();
}

eRenderShape LiquidTile::getRenderShape() const
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

float LiquidTile::getSlopeAngle(TileSource* source, const TilePos& pos, const Material* pMtl)
{
	Vec3 vec;
	if (pMtl == Material::water)
		vec = ((LiquidTile*)Tile::water)->getFlow(source, pos);
	if (pMtl == Material::lava)
		vec = ((LiquidTile*)Tile::lava)->getFlow(source, pos);

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

void LiquidTile::handleEntityInside(TileSource* source, const TilePos& pos, const Entity* pEnt, Vec3& vec)
{
	vec += getFlow(source, pos);
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

void LiquidTile::neighborChanged(TileSource* source, const TilePos& pos, TileID tile)
{
	updateLiquid(source, pos);
}

void LiquidTile::onPlace(TileSource* source, const TilePos& pos)
{
	updateLiquid(source, pos);
}

bool LiquidTile::shouldRenderFace(TileSource* source, const TilePos& pos, Facing::Name face) const
{
	Material* pMtl = source->getMaterial(pos);
	if (pMtl == m_pMaterial || pMtl == Material::ice)
		return false;

	if (face == Facing::UP)
		return true;

	return Tile::shouldRenderFace(source, pos, face);
}

void LiquidTile::tick(TileSource* source, const TilePos& pos, Random* random)
{
}

void LiquidTile::updateLiquid(TileSource* source, const TilePos& pos)
{
	if (source->getTile(pos) != m_ID)
		return;

	if (m_pMaterial != Material::lava)
		// such interactions do not apply to water
		return;

	if (source->getMaterial(pos.north()) == Material::water ||
		source->getMaterial(pos.south()) == Material::water ||
		source->getMaterial(pos.west()) == Material::water ||
		source->getMaterial(pos.east()) == Material::water ||
		source->getMaterial(pos.above()) == Material::water)
	{
		Tile* newTile;
		TileData data = source->getData(pos);

		if (data == 0)
		{
			newTile = Tile::obsidian;
		}
		else if (data > 4)
		{
			// @NOTE: huh?
			fizz(source, pos);
			return;
		}
		else
		{
			newTile = Tile::stoneBrick;
		}

		source->setTile(pos, newTile->m_ID);

		fizz(source, pos);
	}
}
