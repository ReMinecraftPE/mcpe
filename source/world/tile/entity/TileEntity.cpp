#include "TileEntity.hpp"
#include "common/Logger.hpp"
#include "world/level/Level.hpp"

TileEntity::TileEntity() : m_bRemove(false), m_pLevel(nullptr)
{
}

TileEntity::~TileEntity()
{
}

TileEntity* TileEntity::LoadTileEntity(const CompoundTag& tag)
{
    if (!tag.contains("id"))
    {
        LOG_W("Skipping TileEntity with no id!");
        return nullptr;
    }

    std::string id = tag.getString("id");
    const TileEntityType* type = TileEntityFactory::getType(id);

    if (!type)
    {
        LOG_I("Skipping TileEntity with id %s", id.c_str());
        return nullptr;
    }

    TileEntity* newEnt = type->newTileEntity();
    newEnt->load(tag);
    return newEnt;
}

void TileEntity::load(const CompoundTag& tag)
{
    m_pos.x = tag.getInt32("x");
    m_pos.y = tag.getInt32("y");
    m_pos.z = tag.getInt32("z");
}

void TileEntity::save(CompoundTag& tag) const
{
    tag.putString("id", getId());
    tag.putInt32("x", m_pos.x);
    tag.putInt32("y", m_pos.y);
    tag.putInt32("z", m_pos.z);
}

void TileEntity::tick()
{
}

Packet* TileEntity::getUpdatePacket()
{
    return nullptr;
}

bool TileEntity::isRemoved() const
{
    return m_bRemove;
}

void TileEntity::setRemoved()
{
    m_bRemove = true;
}

void TileEntity::clearRemoved()
{
    m_bRemove = false;
}

int TileEntity::getData() const
{
    return m_pLevel->getData(m_pos);
}

void TileEntity::setData(int data)
{
    m_pLevel->setData(m_pos, data);
}

void TileEntity::setChanged()
{
    // @TODO: tileEntityChanged
    /*
    if (m_pLevel)
        m_pLevel->tileEntityChanged(m_pos, this);
    */
}

float TileEntity::distanceToSqr(const Vec3& vec) const
{
    return (m_pos + 0.5f).distanceToSqr(vec);
}

Tile* TileEntity::getTile() const
{
    return Tile::tiles[m_pLevel->getTile(m_pos)];
}

std::string TileEntity::getId() const
{
    return (m_pType) ? m_pType->getName() : "Unknown";
}