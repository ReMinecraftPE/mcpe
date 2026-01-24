#pragma once

#include "TileItem.hpp"

class AuxTileItem : public TileItem
{
public:
	AuxTileItem(int id);
	int getIcon(const ItemStack*) const override;
	TileData getLevelDataForAuxValue(int x) const override;
};
