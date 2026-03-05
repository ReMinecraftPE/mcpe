/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "FireTile.hpp"
#include "world/level/Level.hpp"
#include "world/level/TileSource.hpp"
#include "world/level/TileTickingQueue.hpp"

FireTile::FireTile(int ID, int texture) : Tile(ID, texture, Material::fire)
{
	// @BUG: Uninitialized memory can cause the odds of normally inflammable items to go
	// out of reasonable limits, therefore causing the bug demonstrated in the following video:
	// https://www.youtube.com/watch?v=3hrz7KK2EJs
#ifndef ORIGINAL_CODE
	memset(m_igniteOdds, 0, sizeof m_igniteOdds);
	memset(m_burnOdds, 0, sizeof m_burnOdds);
#endif

	m_igniteOdds[Tile::wood->m_ID]      = 5;
	m_burnOdds  [Tile::wood->m_ID]      = 20;
	m_igniteOdds[Tile::treeTrunk->m_ID] = 5;
	m_burnOdds  [Tile::treeTrunk->m_ID] = 5;
	m_igniteOdds[Tile::leaves->m_ID]    = 30;
	m_burnOdds  [Tile::leaves->m_ID]    = 60;
	m_igniteOdds[Tile::tnt->m_ID]       = 15;
	m_burnOdds  [Tile::tnt->m_ID]       = 100;
	m_igniteOdds[Tile::cloth->m_ID]     = 30;
	m_burnOdds  [Tile::cloth->m_ID]     = 60;
	// @NOTE: Not setting the other cloths' properties

	m_renderLayer = RENDER_LAYER_ALPHATEST;

	setTicking(true);
}

eRenderShape FireTile::getRenderShape() const
{
	// @BUG: Since the shape is set to FIRE, but TileRenderer doesn't handle it,
	// fire is invisible in this version of Minecraft.
	return SHAPE_FIRE;
}

bool FireTile::isSolidRender() const
{
	return false;
}

bool FireTile::isCubeShaped() const
{
	return false;
}

AABB* FireTile::getAABB(TileSource* source, const TilePos& pos)
{
	return nullptr;
}

int FireTile::getResourceCount(Random* random) const
{
	return 0;
}

int FireTile::getTickDelay() const
{
	return 10;
}

void FireTile::animateTick(TileSource* source, const TilePos& pos, Random* random)
{
	Level& level = source->getLevel();

	// @TODO: Mark Tile::fire as FireTile* instead of Tile*
	FireTile* pFireTile = (FireTile*)Tile::fire;
	if (source->isSolidBlockingTile(pos.below()) || pFireTile->canBurn(source, pos.below()))
	{
		for (int i = 0; i < 3; i++)
		{
			level.addParticle("largesmoke", Vec3(float(pos.x) + random->nextFloat(), float(pos.y) + random->nextFloat() * 0.5f + 0.5f, float(pos.z) + random->nextFloat()));
		}

		return;
	}

	if (pFireTile->canBurn(source, pos.west()))
		level.addParticle("largesmoke", Vec3(float(pos.x) + random->nextFloat() * 0.1f, float(pos.y) + random->nextFloat(), float(pos.z) + random->nextFloat()));
	if (pFireTile->canBurn(source, pos.east()))
		level.addParticle("largesmoke", Vec3(float(pos.x + 1) - random->nextFloat() * 0.1f, float(pos.y) + random->nextFloat(), float(pos.z) + random->nextFloat()));
	if (pFireTile->canBurn(source, pos.north()))
		level.addParticle("largesmoke", Vec3(float(pos.x) + random->nextFloat(), float(pos.y) + random->nextFloat(), float(pos.z) + random->nextFloat() * 0.1f));
	if (pFireTile->canBurn(source, pos.south()))
		level.addParticle("largesmoke", Vec3(float(pos.x) + random->nextFloat(), float(pos.y) + random->nextFloat(), float(pos.z + 1) - random->nextFloat() * 0.1f));
	if (pFireTile->canBurn(source, pos.above()))
		level.addParticle("largesmoke", Vec3(float(pos.x) + random->nextFloat(), float(pos.y + 1) - random->nextFloat() * 0.1f, float(pos.z) + random->nextFloat()));
}

void FireTile::checkBurn(TileSource* source, const TilePos& pos, int thing, Random* random)
{
	if (m_burnOdds[source->getTile(pos)] > int(random->nextInt(thing)))
	{
		TileID tid = source->getTile(pos);

		TileID newTile = m_ID;
		if (random->nextInt(2))
			newTile = TILE_AIR;

		source->setTile(pos, newTile);
		if (tid == Tile::tnt->m_ID)
			Tile::tnt->destroy(source, pos, 0);
	}
}

int FireTile::getFireOdds(TileSource* source, const TilePos& pos)
{
	if (!source->isEmptyTile(pos))
		return 0;

	int odds = m_igniteOdds[source->getTile(pos.east())], o;
	if (odds < 0)
		odds = 0;

	o = m_igniteOdds[source->getTile(pos.west())];
	if (odds < o) odds = o;
	o = m_igniteOdds[source->getTile(pos.below())];
	if (odds < o) odds = o;
	o = m_igniteOdds[source->getTile(pos.above())];
	if (odds < o) odds = o;
	o = m_igniteOdds[source->getTile(pos.north())];
	if (odds < o) odds = o;
	o = m_igniteOdds[source->getTile(pos.south())];
	if (odds < o) odds = o;

	return odds;
}

bool FireTile::isValidFireLocation(TileSource* source, const TilePos& pos) const
{
	if (canBurn(source, pos.east())) return true;
	if (canBurn(source, pos.west())) return true;
	if (canBurn(source, pos.below())) return true;
	if (canBurn(source, pos.above())) return true;
	if (canBurn(source, pos.north())) return true;
	if (canBurn(source, pos.south())) return true;
	return false;
}

bool FireTile::mayPick() const
{
	return false;
}

bool FireTile::mayPlace(TileSource* source, const TilePos& pos) const
{
	// @NOTE: This is useless as you usually don't 'place' fire.
	if (source->isSolidBlockingTile(pos.below()))
		return true;

	return isValidFireLocation(source, pos);
}

void FireTile::neighborChanged(TileSource* source, const TilePos& pos, TileID tile)
{
	if (!source->isSolidBlockingTile(pos.below()) && !isValidFireLocation(source, pos))
		source->setTile(pos, TILE_AIR);
}

void FireTile::onPlace(TileSource* source, const TilePos& pos)
{
	// @NOTE: Unused return result
	source->getTile(pos.below());

	if (!source->isSolidBlockingTile(pos.below()) && !isValidFireLocation(source, pos))
	{
		source->setTile(pos, TILE_AIR);
		return;
	}

	source->getTickQueue(pos)->add(source, pos, m_ID, getTickDelay());
}

void FireTile::tick(TileSource* source, const TilePos& pos, Random* random)
{
	if (source->getTile(pos.below()) == Tile::netherrack->m_ID)
	{
		source->getTickQueue(pos)->add(source, pos, m_ID, getTickDelay());
		return;
	}
	
	TileData data = source->getData(pos);
	if (data <= 14)
	{
		source->setTileAndData(pos, FullTile(this, data + 1));
		source->getTickQueue(pos)->add(source, pos, m_ID, getTickDelay());
	}

	if (isValidFireLocation(source, pos))
	{
		if (m_igniteOdds[source->getTile(pos.below())] <= 0 && data == 15 && !random->nextInt(4))
		{
			// just go out
			source->setTile(pos, TILE_AIR);
			return;
		}

		if (data > 2 && (data & 1) == 0)
		{
			checkBurn(source, pos.east(), 300, random);
			checkBurn(source, pos.west(), 300, random);
			checkBurn(source, pos.below(), 250, random);
			checkBurn(source, pos.above(), 250, random);
			checkBurn(source, pos.north(), 300, random);
			checkBurn(source, pos.south(), 300, random);

			TilePos tp(pos.north());
			for (tp.x = pos.x - 1; tp.x <= pos.x + 1; tp.x++)
			{
				for (tp.z = pos.z - 1; tp.z <= pos.z + 1; tp.z++)
				{
					int thing = -100;
					for (tp.y = pos.y - 1; tp.y <= pos.y + 4; tp.y++, thing += 100)
					{
						if (tp == pos)
							continue;

						int thing2 = pos.y + 1 >= tp.y ? 100 : thing;
						int odds = getFireOdds(source, tp);
						if (odds > 0 && odds >= int(random->nextInt(thing2)))
							source->setTile(tp, m_ID);
					}
				}
			}
		}
		if (data == 15)
		{
			checkBurn(source, pos.east(), 1, random);
			checkBurn(source, pos.west(), 1, random);
			checkBurn(source, pos.below(), 1, random);
			checkBurn(source, pos.above(), 1, random);
			checkBurn(source, pos.north(), 1, random);
			checkBurn(source, pos.south(), 1, random);
		}
	}
	else if (!source->isSolidBlockingTile(pos.below()) || data > 3)
	{
		source->setTile(pos, TILE_AIR);
	}
}

bool FireTile::canBurn(TileSource* source, const TilePos& pos) const
{
	return m_igniteOdds[source->getTile(pos)] > 0;
}
