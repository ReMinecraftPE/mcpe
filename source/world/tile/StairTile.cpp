/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "StairTile.hpp"
#include "world/level/Level.hpp"

// @NOTE: All this work for some stairs; damn

StairTile::StairTile(int id, Tile* pTile) : Tile(id, pTile->m_TextureFrame, pTile->m_pMaterial)
{
	m_pParent = pTile;

	// inherit these properties from the parent tile
	setDestroyTime(pTile->m_hardness);
	setExplodeable(pTile->m_blastResistance / 3.0f);
	setSoundType(*pTile->m_pSound);
}

void StairTile::addAABBs(Level* level, int x, int y, int z, const AABB* aabb, std::vector<AABB>& out)
{
	int data = level->getData(x, y, z);
	switch (data)
	{
		case 0:
		{
			setShape(0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 1.0f);
			Tile::addAABBs(level, x, y, z, aabb, out);
			setShape(0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
			Tile::addAABBs(level, x, y, z, aabb, out);
			break;
		}
		case 1:
		{
			setShape(0.0f, 0.0f, 0.0f, 0.5f, 1.0f, 1.0f);
			Tile::addAABBs(level, x, y, z, aabb, out);
			setShape(0.5f, 0.0f, 0.0f, 1.0f, 0.5f, 1.0f);
			Tile::addAABBs(level, x, y, z, aabb, out);
			break;
		}
		case 2:
		{
			setShape(0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f);
			Tile::addAABBs(level, x, y, z, aabb, out);
			setShape(0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f);
			Tile::addAABBs(level, x, y, z, aabb, out);
			break;
		}
		case 3:
		{
			setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f);
			Tile::addAABBs(level, x, y, z, aabb, out);
			setShape(0.0f, 0.0f, 0.5f, 1.0f, 0.5f, 1.0f);
			Tile::addAABBs(level, x, y, z, aabb, out);
			break;
		}
	}

	setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
}

bool StairTile::isSolidRender()
{
	return false;
}

bool StairTile::isCubeShaped()
{
	return false;
}

int StairTile::getRenderShape()
{
	return SHAPE_STAIRS;
}

void StairTile::addLights(Level* level, int x, int y, int z)
{
	m_pParent->addLights(level, x, y, z);
}

void StairTile::animateTick(Level* level, int x, int y, int z, Random* random)
{
	m_pParent->animateTick(level, x, y, z, random);
}

void StairTile::updateShape(LevelSource* level, int x, int y, int z)
{
	setShape(0, 0, 0, 1, 1, 1);
}

float StairTile::getBrightness(LevelSource* level, int x, int y, int z)
{
	return m_pParent->getBrightness(level, x, y, z);
}

int StairTile::getTexture(int a)
{
	return m_pParent->getTexture(a);
}

int StairTile::getTexture(int a, int b)
{
	return m_pParent->getTexture(a, b);
}

int StairTile::getTexture(LevelSource* level, int x, int y, int z, int data)
{
	return m_pParent->getTexture(level, x, y, z, data);
}

AABB StairTile::getTileAABB(Level* level, int x, int y, int z)
{
	return m_pParent->getTileAABB(level, x, y, z);
}

bool StairTile::mayPick()
{
	return m_pParent->mayPick();
}

bool StairTile::mayPick(int a, bool b)
{
	return m_pParent->mayPick(a, b);
}

bool StairTile::mayPlace(Level* level, int x, int y, int z)
{
	return m_pParent->mayPlace(level, x, y, z);
}

int StairTile::getTickDelay()
{
	return m_pParent->getTickDelay();
}

void StairTile::tick(Level* level, int x, int y, int z, Random* random)
{
	m_pParent->tick(level, x, y, z, random);
}

void StairTile::destroy(Level* level, int x, int y, int z, int dir)
{
	m_pParent->destroy(level, x, y, z, dir);
}

void StairTile::onPlace(Level* level, int x, int y, int z)
{
	neighborChanged(level, x, y, z, 0);
	m_pParent->onPlace(level, x, y, z);
}

void StairTile::onRemove(Level* level, int x, int y, int z)
{
	m_pParent->onRemove(level, x, y, z);
}

int StairTile::getResource(int a, Random* random)
{
	return m_pParent->getResource(a, random);
}

int StairTile::getResourceCount(Random* random)
{
	return m_pParent->getResourceCount(random);
}

void StairTile::spawnResources(Level* level, int x, int y, int z, int d)
{
	m_pParent->spawnResources(level, x, y, z, d);
}

void StairTile::spawnResources(Level* level, int x, int y, int z, int d, float f)
{
	m_pParent->spawnResources(level, x, y, z, d, f);
}

float StairTile::getExplosionResistance(Entity* entity)
{
	return m_pParent->getExplosionResistance(entity);
}

void StairTile::wasExploded(Level* level, int x, int y, int z)
{
	return m_pParent->wasExploded(level, x, y, z);
}

int StairTile::getRenderLayer()
{
	return m_pParent->getRenderLayer();
}

int StairTile::use(Level* level, int x, int y, int z, Player* player)
{
	return m_pParent->use(level, x, y, z, player);
}

void StairTile::stepOn(Level* level, int x, int y, int z, Entity* entity)
{
	m_pParent->stepOn(level, x, y, z, entity);
}

void StairTile::setPlacedBy(Level* level, int x, int y, int z, Mob* mob)
{
	int rot = Mth::floor(0.5f + (mob->m_yaw * 4.0f / 360.0f)) & 3;

	int data = 0;

	switch (rot)
	{
		case 0: data = 2; break;
		case 1: data = 1; break;
		case 2: data = 3; break;
	}

	level->setData(x, y, z, data);
}

void StairTile::prepareRender(Level* level, int x, int y, int z)
{
	return m_pParent->prepareRender(level, x, y, z);
}

void StairTile::attack(Level* level, int x, int y, int z, Player* player)
{
	m_pParent->attack(level, x, y, z, player);
}

void StairTile::handleEntityInside(Level* level, int x, int y, int z, Entity* entity, Vec3& vec)
{
	m_pParent->handleEntityInside(level, x, y, z, entity, vec);
}
