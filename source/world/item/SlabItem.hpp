#pragma once

#include "AuxTileItem.hpp"

class SlabItem : public AuxTileItem
{
public:
	SlabItem(int id);

public:
	std::string getDescriptionId(ItemStack* item) const override;
};
