/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TileItem.hpp"
#include "world/level/Level.hpp"
#include "world/tile/Tile.hpp"

TileItem::TileItem(int id) : Item(id)
{
	id += 256;
	m_tile = id;
	m_icon = Tile::tiles[id]->getTexture(Facing::NORTH);
}

std::string TileItem::getDescriptionId() const
{
	return Tile::tiles[m_tile]->getDescriptionId();
}

std::string TileItem::getDescriptionId(ItemInstance* instance) const
{
	return Tile::tiles[m_tile]->getDescriptionId();
}

bool TileItem::useOn(ItemInstance* instance, Player* player, Level* level, const TilePos& pos, Facing::Name face) const
{
	TilePos tp(pos);

	if (level->getTile(pos) == Tile::topSnow->m_ID)
	{
		face = Facing::DOWN;
	}
	else switch (face)
	{
		case Facing::DOWN: tp.y--; break;
		case Facing::UP: tp.y++; break;
		case Facing::NORTH: tp.z--; break;
		case Facing::SOUTH: tp.z++; break;
		case Facing::WEST: tp.x--; break;
		case Facing::EAST: tp.x++; break;
	}

	if (instance->m_count == 0)
		return false;

	if (!level->mayPlace(m_tile, tp, false))
		return false;

	Tile* pTile = Tile::tiles[m_tile];

	if (!level->setTileAndData(tp, m_tile, getLevelDataForAuxValue(instance->getAuxValue())))
		return true;

	Tile::tiles[m_tile]->setPlacedOnFace(level, tp, face);
	Tile::tiles[m_tile]->setPlacedBy(level, tp, player);

	level->playSound(
		Vec3(tp) + 0.5f,
		"step." + pTile->m_pSound->m_name,
		(pTile->m_pSound->volume + 1.0f) * 0.5f,
		pTile->m_pSound->pitch * 0.8f
	);

	player->useItem(*instance);
	return true;
}
