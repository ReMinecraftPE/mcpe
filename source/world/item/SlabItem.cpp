#include "SlabItem.hpp"
#include "world/tile/StoneSlabTile.hpp"

SlabItem::SlabItem(int id) : AuxTileItem(id)
{
}

std::string SlabItem::getDescriptionId(ItemInstance* item) const
{
	int var2 = item->getAuxValue();
	if (var2 < 0 || var2 >= 4)
	{
		var2 = 0;
	}

	std::string name;
	switch (var2)
	{
		case StoneSlabTile::STONE:
			name = "stone";
			break;
		case StoneSlabTile::SAND:
			name = "sand";
			break;
		case StoneSlabTile::WOOD:
			name = "wood";
			break;
		case StoneSlabTile::COBBLE:
			name = "cobble";
			break;
	}

	return AuxTileItem::getDescriptionId(item) + "." + name;
}
