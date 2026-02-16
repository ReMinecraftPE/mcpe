#pragma once

#include "TileEntityType.hpp"
#include "world/level/TilePos.hpp"
#include "world/phys/Vec3.hpp"
#include "nbt/CompoundTag.hpp"

class Tile;
class Level;
class Packet;

class TileEntity
{
public:
    TileEntity();
    virtual ~TileEntity();

protected:
    virtual std::string getId() const;

public:
    virtual void load(const CompoundTag& tag);
    virtual void save(CompoundTag& tag) const;
    virtual void tick();
    virtual Packet* getUpdatePacket();
    const TileEntityType* getType() const;
    virtual int getData() const;
    virtual Tile* getTile() const;

public:
    static TileEntity* LoadTileEntity(const CompoundTag& tag);

public:
    float distanceToSqr(const Vec3& vec) const;
    bool isRemoved() const;
    void setRemoved();
    void clearRemoved();

    void setData(int data);
    void setChanged();

protected:
    const TileEntityType* m_pType;
    bool m_bRemove;

public:
    Level* m_pLevel;
    TilePos m_pos;
};

