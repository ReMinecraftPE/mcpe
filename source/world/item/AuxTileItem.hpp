#pragma once

#include "TileItem.hpp"

class AuxTileItem : public TileItem
{
public:
	AuxTileItem(int id);
	int getIcon(const ItemInstance*) const override;
	TileData getLevelDataForAuxValue(int x) const override;
};
