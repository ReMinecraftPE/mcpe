/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "StairTile.hpp"
#include "world/level/Level.hpp"

// @NOTE: All this work for some stairs; damn

// Correct me if I'm wrong, but can we not just use object inheritance for this?
// - Brent

StairTile::StairTile(int id, Tile* pTile) : Tile(id, pTile->m_TextureFrame, pTile->m_pMaterial)
{
	m_pParent = pTile;

	// inherit these properties from the parent tile
	setDestroyTime(pTile->m_hardness);
	setExplodeable(pTile->m_blastResistance / 3.0f);
	setSoundType(*pTile->m_pSound);
}

void StairTile::addAABBs(const Level* level, const TilePos& pos, const AABB* aabb, std::vector<AABB>& out)
{
	int data = level->getData(pos);
	switch (data)
	{
		case 0:
		{
			setShape(0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 1.0f);
			Tile::addAABBs(level, pos, aabb, out);
			setShape(0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
			Tile::addAABBs(level, pos, aabb, out);
			break;
		}
		case 1:
		{
			setShape(0.0f, 0.0f, 0.0f, 0.5f, 1.0f, 1.0f);
			Tile::addAABBs(level, pos, aabb, out);
			setShape(0.5f, 0.0f, 0.0f, 1.0f, 0.5f, 1.0f);
			Tile::addAABBs(level, pos, aabb, out);
			break;
		}
		case 2:
		{
			setShape(0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f);
			Tile::addAABBs(level, pos, aabb, out);
			setShape(0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f);
			Tile::addAABBs(level, pos, aabb, out);
			break;
		}
		case 3:
		{
			setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f);
			Tile::addAABBs(level, pos, aabb, out);
			setShape(0.0f, 0.0f, 0.5f, 1.0f, 0.5f, 1.0f);
			Tile::addAABBs(level, pos, aabb, out);
			break;
		}
	}

	setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
}

bool StairTile::isSolidRender() const
{
	return false;
}

bool StairTile::isCubeShaped() const
{
	return false;
}

int StairTile::getRenderShape() const
{
	return SHAPE_STAIRS;
}

void StairTile::addLights(Level* level, const TilePos& pos)
{
	m_pParent->addLights(level, pos);
}

void StairTile::animateTick(Level* level, const TilePos& pos, Random* random)
{
	m_pParent->animateTick(level, pos, random);
}

void StairTile::updateShape(const LevelSource* level, const TilePos& pos)
{
	setShape(0, 0, 0, 1, 1, 1);
}

float StairTile::getBrightness(const LevelSource* level, const TilePos& pos) const
{
	return m_pParent->getBrightness(level, pos);
}

int StairTile::getTexture(Facing::Name face) const
{
	return m_pParent->getTexture(face);
}

int StairTile::getTexture(Facing::Name face, TileData data) const
{
	return m_pParent->getTexture(face, data);
}

int StairTile::getTexture(const LevelSource* level, const TilePos& pos, Facing::Name face) const
{
	return m_pParent->getTexture(level, pos, face);
}

AABB StairTile::getTileAABB(const Level* level, const TilePos& pos)
{
	return m_pParent->getTileAABB(level, pos);
}

bool StairTile::mayPick() const
{
	return m_pParent->mayPick();
}

bool StairTile::mayPick(TileData data, bool b) const
{
	return m_pParent->mayPick(data, b);
}

bool StairTile::mayPlace(const Level* level, const TilePos& pos) const
{
	return m_pParent->mayPlace(level, pos);
}

int StairTile::getTickDelay() const
{
	return m_pParent->getTickDelay();
}

void StairTile::tick(Level* level, const TilePos& pos, Random* random)
{
	m_pParent->tick(level, pos, random);
}

void StairTile::destroy(Level* level, const TilePos& pos, TileData data)
{
	m_pParent->destroy(level, pos, data);
}

void StairTile::onPlace(Level* level, const TilePos& pos)
{
	neighborChanged(level, pos, Facing::DOWN);
	m_pParent->onPlace(level, pos);
}

void StairTile::onRemove(Level* level, const TilePos& pos)
{
	m_pParent->onRemove(level, pos);
}

int StairTile::getResource(TileData data, Random* random) const
{
	return m_pParent->getResource(data, random);
}

int StairTile::getResourceCount(Random* random) const
{
	return m_pParent->getResourceCount(random);
}

void StairTile::spawnResources(Level* level, const TilePos& pos, TileData data)
{
	m_pParent->spawnResources(level, pos, data);
}

void StairTile::spawnResources(Level* level, const TilePos& pos, TileData data, float f)
{
	m_pParent->spawnResources(level, pos, data, f);
}

float StairTile::getExplosionResistance(Entity* entity) const
{
	return m_pParent->getExplosionResistance(entity);
}

void StairTile::wasExploded(Level* level, const TilePos& pos)
{
	return m_pParent->wasExploded(level, pos);
}

int StairTile::getRenderLayer() const
{
	return m_pParent->getRenderLayer();
}

bool StairTile::use(Level* level, const TilePos& pos, Player* player)
{
	return m_pParent->use(level, pos, player);
}

void StairTile::stepOn(Level* level, const TilePos& pos, Entity* entity)
{
	m_pParent->stepOn(level, pos, entity);
}

void StairTile::setPlacedBy(Level* level, const TilePos& pos, Mob* mob)
{
	int rot = Mth::floor(0.5f + (mob->m_rot.x * 4.0f / 360.0f)) & 3;

	TileData data = 0;

	switch (rot)
	{
		case 0: data = 2; break;
		case 1: data = 1; break;
		case 2: data = 3; break;
	}

	level->setData(pos, data);
}

void StairTile::prepareRender(Level* level, const TilePos& pos)
{
	return m_pParent->prepareRender(level, pos);
}

void StairTile::attack(Level* level, const TilePos& pos, Player* player)
{
	m_pParent->attack(level, pos, player);
}

void StairTile::handleEntityInside(Level* level, const TilePos& pos, const Entity* entity, Vec3& vec)
{
	m_pParent->handleEntityInside(level, pos, entity, vec);
}
