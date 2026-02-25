#include "BedItem.hpp"
#include "world/level/Level.hpp"
#include "world/entity/Player.hpp"
#include "world/tile/Tile.hpp"
#include "world/tile/BedTile.hpp"

BedItem::BedItem(int id) : Item(id)
{
	m_maxStackSize = 1;
}

bool BedItem::useOn(ItemStack* inst, Player* player, Level* level, const TilePos& pos, Facing::Name face) const
{
	if (face != Facing::UP) {
		return false;
	}

	TilePos tp = pos.above();
	Tile* bedTile = Tile::bed;
	int dir = Mth::floor((player->m_rot.x * 4.0f / 360.0f) + 0.5f) & 3;
	TilePos offTp(tp);
	if (dir == 0) {
		offTp.z += 1;
	}

	if (dir == 1) {
		offTp.x -= 1;
	}

	if (dir == 2) {
		offTp.z -= 1;
	}

	if (dir == 3) {
		offTp.x += 1;
	}

	if (level->isEmptyTile(tp) && level->isEmptyTile(offTp) && 
		level->isSolidTile(tp.below()) && level->isSolidTile(offTp.below())) {
		
		level->setTileAndData(tp, bedTile->m_ID, dir);
		level->setTileAndData(offTp, bedTile->m_ID, dir + 8);
		--inst->m_count;
		return true;
	}
	
	return false;
}
