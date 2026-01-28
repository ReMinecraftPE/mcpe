#pragma once

#include "AuxTileItem.hpp"

class SlabItem : public AuxTileItem
{
public:
	SlabItem(int id);
	std::string getDescriptionId(ItemStack* item) const override;
};
