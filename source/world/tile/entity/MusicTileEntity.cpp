#include "MusicTileEntity.hpp"
#include "world/level/Level.hpp"

MusicTileEntity::MusicTileEntity() : TileEntity(), m_note(0), m_bOn(false)
{
    m_pType = TileEntityType::chest;
}

void MusicTileEntity::load(const CompoundTag& tag)
{
    TileEntity::load(tag);
    m_note = static_cast<uint8_t>(Mth::clamp(tag.getInt8("note"), 0, 24));
}

void MusicTileEntity::save(CompoundTag& tag) const
{
    TileEntity::save(tag);
    tag.putInt8("note", m_note);
}

void MusicTileEntity::tune()
{
    m_note = (m_note + 1) % 25;
    setChanged();
}

void MusicTileEntity::play(Level* pLevel, const TilePos& pos)
{
    // noteblocks only play if the block above them is air
    if (pLevel->getMaterial(TilePos(pos.x, pos.y + 1, pos.z)) != Material::air)
        return;

    int instrument = 0;
    Material* below = pLevel->getMaterial(TilePos(pos.x, pos.y - 1, pos.z));

    /*
    // WHY
    switch (below)
    {
    case Material::stone:
        instrument = 1;
        break;
    case Material::sand:
        instrument = 2;
        break;
    case Material::glass:
        instrument = 3;
        break;
    case Material::wood:
        instrument = 4;
        break;
    default:
        break;
    }
    */

    if (below == Material::stone)
        instrument = 1;
    else if (below == Material::sand)
        instrument = 2;
    else if (below == Material::glass)
        instrument = 3;
    else if (below == Material::wood)
        instrument = 4;

    level->tileEvent(pos, instrument, m_note);
}