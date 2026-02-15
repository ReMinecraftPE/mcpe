#pragma once

#include "TileEntity.hpp"
#include "world/inventory/SimpleContainer.hpp"
#include "world/level/Level.hpp"

class FurnaceTileEntity : public SimpleContainer, public TileEntity
{
public:
    FurnaceTileEntity();

private:
    bool _canBurn();
    void _burn();

public:
    void tick() override;
    bool stillValid(Player* player) const override;
    void setChanged() override;
    void load(const CompoundTag& tag) override;
    void save(CompoundTag& tag) const override;
    std::string getName() const override;

public:
    int getBurnProgress(int height);
    int getLitProgress(int height);
    bool isLit();

public:
    int m_litTime;
    int m_litDuration;
    int m_tickCount;
};
