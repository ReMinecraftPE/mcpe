/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "StairTile.hpp"
#include "world/level/Level.hpp"
#include "world/level/TileSource.hpp"

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

void StairTile::addAABBs(TileSource* source, const TilePos& pos, const AABB* aabb, std::vector<AABB>& out)
{
	TileData data = source->getData(pos);
	switch (data)
	{
		case 0:
		{
			setShape(0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 1.0f);
			Tile::addAABBs(source, pos, aabb, out);
			setShape(0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
			Tile::addAABBs(source, pos, aabb, out);
			break;
		}
		case 1:
		{
			setShape(0.0f, 0.0f, 0.0f, 0.5f, 1.0f, 1.0f);
			Tile::addAABBs(source, pos, aabb, out);
			setShape(0.5f, 0.0f, 0.0f, 1.0f, 0.5f, 1.0f);
			Tile::addAABBs(source, pos, aabb, out);
			break;
		}
		case 2:
		{
			setShape(0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f);
			Tile::addAABBs(source, pos, aabb, out);
			setShape(0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f);
			Tile::addAABBs(source, pos, aabb, out);
			break;
		}
		case 3:
		{
			setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f);
			Tile::addAABBs(source, pos, aabb, out);
			setShape(0.0f, 0.0f, 0.5f, 1.0f, 0.5f, 1.0f);
			Tile::addAABBs(source, pos, aabb, out);
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

eRenderShape StairTile::getRenderShape() const
{
	return SHAPE_STAIRS;
}

void StairTile::addLights(TileSource* source, const TilePos& pos)
{
	m_pParent->addLights(source, pos);
}

void StairTile::animateTick(TileSource* source, const TilePos& pos, Random* random)
{
	m_pParent->animateTick(source, pos, random);
}

void StairTile::updateShape(TileSource* source, const TilePos& pos)
{
	setShape(0, 0, 0, 1, 1, 1);
}

float StairTile::getBrightness(TileSource* source, const TilePos& pos) const
{
	return m_pParent->getBrightness(source, pos);
}

int StairTile::getTexture(Facing::Name face) const
{
	return m_pParent->getTexture(face);
}

int StairTile::getTexture(Facing::Name face, TileData data) const
{
	return m_pParent->getTexture(face, data);
}

int StairTile::getTexture(TileSource* source, const TilePos& pos, Facing::Name face) const
{
	return m_pParent->getTexture(source, pos, face);
}

AABB StairTile::getTileAABB(TileSource* source, const TilePos& pos)
{
	return m_pParent->getTileAABB(source, pos);
}

bool StairTile::mayPick() const
{
	return m_pParent->mayPick();
}

bool StairTile::mayPick(TileData data, bool b) const
{
	return m_pParent->mayPick(data, b);
}

bool StairTile::mayPlace(TileSource* source, const TilePos& pos) const
{
	return m_pParent->mayPlace(source, pos);
}

int StairTile::getTickDelay() const
{
	return m_pParent->getTickDelay();
}

void StairTile::tick(TileSource* source, const TilePos& pos, Random* random)
{
	m_pParent->tick(source, pos, random);
}

void StairTile::destroy(TileSource* source, const TilePos& pos, TileData data)
{
	m_pParent->destroy(source, pos, data);
}

void StairTile::onPlace(TileSource* source, const TilePos& pos)
{
	neighborChanged(source, pos, Facing::DOWN);
	m_pParent->onPlace(source, pos);
}

void StairTile::onRemove(TileSource* source, const TilePos& pos)
{
	m_pParent->onRemove(source, pos);
}

float StairTile::getExplosionResistance(Entity* entity) const
{
	return m_pParent->getExplosionResistance(entity);
}

void StairTile::wasExploded(TileSource* source, const TilePos& pos)
{
	return m_pParent->wasExploded(source, pos);
}

Tile::RenderLayer StairTile::getRenderLayer(TileSource* source, const TilePos& pos) const
{
	return m_pParent->getRenderLayer(source, pos);
}

bool StairTile::use(TileSource* source, const TilePos& pos, Player* player)
{
	return m_pParent->use(source, pos, player);
}

void StairTile::stepOn(TileSource* source, const TilePos& pos, Entity* entity)
{
	m_pParent->stepOn(source, pos, entity);
}

void StairTile::setPlacedBy(TileSource* source, const TilePos& pos, Mob* mob)
{
	int rot = Mth::floor(0.5f + (mob->m_rot.x * 4.0f / 360.0f)) & 3;

	TileData data = 0;

	switch (rot)
	{
		case 0: data = 2; break;
		case 1: data = 1; break;
		case 2: data = 3; break;
	}

	source->setTileAndData(pos, FullTile(m_ID, data));
}

void StairTile::prepareRender(TileSource* source, const TilePos& pos)
{
	return m_pParent->prepareRender(source, pos);
}

void StairTile::attack(TileSource* source, const TilePos& pos, Player* player)
{
	m_pParent->attack(source, pos, player);
}

void StairTile::handleEntityInside(TileSource* source, const TilePos& pos, const Entity* entity, Vec3& vec)
{
	m_pParent->handleEntityInside(source, pos, entity, vec);
}
