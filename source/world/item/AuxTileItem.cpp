#include "AuxTileItem.hpp"
#include "world/tile/Tile.hpp"

AuxTileItem::AuxTileItem(int id) : TileItem(id)
{
	m_maxDamage = 0;
	m_bStackedByData = true;
}

int AuxTileItem::getIcon(const ItemInstance* item) const
{
	return Tile::tiles[m_itemID]->getTexture(Facing::NORTH, item->getAuxValue());
}

int AuxTileItem::getLevelDataForAuxValue(int x)
{
	return x;
}
