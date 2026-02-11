#include "HoeItem.hpp"
#include "world/entity/Player.hpp"
#include "world/level/Level.hpp"

HoeItem::HoeItem(int id, ToolItem::Tier& tier) : Item(id)
{
	m_maxStackSize = 1;
	m_maxDamage = tier.m_uses;
}

bool HoeItem::useOn(ItemStack* inst, Player* player, Level* level, const TilePos& pos, Facing::Name face) const
{
    int tile = level->getTile(pos);
    int below = level->getTile(pos.above());

    if ((face == Facing::DOWN || below || tile != Tile::grass->m_ID) && tile != Tile::dirt->m_ID)
        return false;

    Tile* newTile = Tile::farmland;
    level->playSound(pos + 0.5f, "step." + newTile->m_pSound->m_name, (newTile->m_pSound->volume + 1.0f) / 2.0f, newTile->m_pSound->pitch * 0.8f);

    if (level->m_bIsClientSide)
        return true;

    level->setTile(pos, newTile->m_ID);
    inst->hurtAndBreak(1, player);
    return true;
}

bool HoeItem::isHandEquipped() const
{
	return true;
}