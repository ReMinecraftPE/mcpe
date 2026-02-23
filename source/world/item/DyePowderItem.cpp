#include "DyePowderItem.hpp"
#include "world/level/Level.hpp"
#include "world/entity/Player.hpp"
#include "world/entity/Sheep.hpp"
#include "world/tile/Tile.hpp"
#include "world/tile/Sapling.hpp"
#include "world/tile/CropsTile.hpp"
#include "world/tile/ClothTile.hpp"
#include "world/entity/EntityType.hpp"
#include "DyeColor.hpp"

DyePowderItem::DyePowderItem(int itemID) : Item(itemID)
{
	m_bStackedByData = true;
	m_maxDamage = 0;
}

int DyePowderItem::getIcon(const ItemStack* item) const
{
	int i = item->getAuxValue();
	return m_icon + i % 8 * 16 + i / 8;
}

std::string DyePowderItem::getDescriptionId() const
{
	return Item::getDescriptionId() + "." + DyeColor::IDS[0];
}

std::string DyePowderItem::getDescriptionId(ItemStack* item) const
{
	return Item::getDescriptionId() + "." + DyeColor::IDS[item->getAuxValue()];
}

bool DyePowderItem::useOn(ItemStack* item, Player* player, Level* level, const TilePos& pos, Facing::Name face) const
{
    // Aux value 15 is bonemeal
    if (item->getAuxValue() == 15)
	{
		TileID tile = level->getTile(pos);
		
		if (tile == Tile::sapling->m_ID)
		{
			(static_cast<Sapling*>(Tile::sapling))->growTree(level, pos, &level->m_random);
			item->m_count--;
			return true;
		}
		
		if (tile == Tile::crops->m_ID)
		{
			static_cast<CropsTile*>(Tile::crops)->growCropsToMax(level, pos);
			item->m_count--;
			return true;
		}
	}
	
	return false;
}

void DyePowderItem::interactEnemy(ItemStack* item, Mob* mob) const
{
	if (!mob->getDescriptor().isType(EntityType::SHEEP))
        return;

    Sheep* sheep = static_cast<Sheep*>(mob);
    if (sheep->isSheared())
        return;

    int color = ClothTile::getColorFromData(item->getAuxValue());
    
    if (sheep->getColor() != color)
    {
        sheep->setColor(color);
        item->m_count--;
    }
}
