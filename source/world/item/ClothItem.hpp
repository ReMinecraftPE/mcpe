#pragma once

#include "TileItem.hpp"

class ClothItem : public TileItem
{
public:
	ClothItem(int id);
	std::string getDescriptionId(ItemInstance* item) override;
	int getIcon(const ItemInstance*) const override;
	int getLevelDataForAuxValue(int x) override;
};
