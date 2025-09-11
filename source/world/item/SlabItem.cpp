#include "SlabItem.hpp"
#include <world/tile/StoneSlabTile.hpp>

SlabItem::SlabItem(int id) : AuxTileItem(id)
{
}

std::string SlabItem::getDescriptionId(ItemInstance* item)
{
	int var2 = item->getAuxValue();
	if (var2 < 0 || var2 >= 4) {
		var2 = 0;
	}

	return AuxTileItem::getDescriptionId(item) + "." + StoneSlabTile::types[var2];
}
