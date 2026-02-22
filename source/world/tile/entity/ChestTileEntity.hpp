#pragma once

#include "TileEntity.hpp"
#include "world/level/Level.hpp"
#include "world/inventory/SimpleContainer.hpp"

class ChestTileEntity : public TileEntity, public SimpleContainer {
public:
    ChestTileEntity();

    void load(const CompoundTag& tag) override;
    void save(CompoundTag& tag) const override;

    bool stillValid(Player* player) const override;

    void setContainerChanged(SlotID slot) override;
};
