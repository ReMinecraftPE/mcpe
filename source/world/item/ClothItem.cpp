#include "ClothItem.hpp"
#include "world/level/Level.hpp"
#include "world/entity/Player.hpp"
#include "world/tile/Tile.hpp"
#include "world/tile/ClothTile.hpp"
//#include "DyeColor.hpp"

ClothItem::ClothItem(int id) : TileItem(id)
{
	m_maxDamage = 0;
	m_bStackedByData = true;
}

std::string ClothItem::getDescriptionId(ItemInstance* item) const
{
	return TileItem::getDescriptionId(item) + "."/* + DyeColor::IDS[ClothTile::getColorFromData(item->getAuxValue())];*/;
}

int ClothItem::getIcon(const ItemInstance* item) const
{
	return Tile::cloth->getTexture(Facing::NORTH, ClothTile::getColorFromData(item->getAuxValue()));
}

TileData ClothItem::getLevelDataForAuxValue(int x) const
{
	return x;
}
