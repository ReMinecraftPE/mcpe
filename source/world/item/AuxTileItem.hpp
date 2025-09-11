#pragma once

#include "TileItem.hpp"

class AuxTileItem : public TileItem
{
public:
	AuxTileItem(int id);
	int getIcon(const ItemInstance*) const override;
	int getLevelDataForAuxValue(int x) override;
};
