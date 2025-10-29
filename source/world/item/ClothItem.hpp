#pragma once

#include "TileItem.hpp"

class ClothItem : public TileItem
{
public:
	ClothItem(int id);
	std::string getDescriptionId(ItemInstance* item) const override;
	int getIcon(const ItemInstance*) const override;
	TileData getLevelDataForAuxValue(int x) const override;
};
