#include "ChestTile.hpp"
#include "world/level/Level.hpp"
#include "world/CompoundContainer.hpp"
#include "world/tile/entity/ChestTileEntity.hpp"

ChestTile::ChestTile(int id, int texture) : Tile(id, texture, Material::wood)
{
    setTicking(true);
}

int ChestTile::getTexture(const LevelSource* level, const TilePos& pos, Facing::Name face) const
{
    if (face == Facing::UP || face == Facing::DOWN) {
        return m_TextureFrame - 1;
    }

    int id_north = level->getTile(pos.north());
    int id_south = level->getTile(pos.south());
    int id_west = level->getTile(pos.west());
    int id_east = level->getTile(pos.east());

    bool isDoubleNS = (id_north == m_ID || id_south == m_ID);
    bool isDoubleWE = (id_west == m_ID || id_east == m_ID);

    if (!isDoubleNS && !isDoubleWE) {
        Facing::Name front = Facing::SOUTH;

        if (Tile::solid[id_north] && !Tile::solid[id_south])
            front = Facing::SOUTH;
        else if (Tile::solid[id_south] && !Tile::solid[id_north])
            front = Facing::NORTH;
        else if (Tile::solid[id_west] && !Tile::solid[id_east])
            front = Facing::EAST;
        else if (Tile::solid[id_east] && !Tile::solid[id_west])
            front = Facing::WEST;

        return (face == front) ? m_TextureFrame + 1 : m_TextureFrame;
    }

    bool left = false;
    Facing::Name front = Facing::SOUTH;

    if (isDoubleWE) {
        left = id_west == m_ID;
        TilePos side = left ? pos.west() : pos.east();

        int id_behind = level->getTile(side.north());
        int id_infront = level->getTile(side.south());

        if ((Tile::solid[id_north] || Tile::solid[id_behind]) && !Tile::solid[id_south] && !Tile::solid[id_infront])
            front = Facing::SOUTH;
        else if ((Tile::solid[id_south] || Tile::solid[id_infront]) && !Tile::solid[id_north] && !Tile::solid[id_behind])
            front = Facing::NORTH;

        if (front == Facing::SOUTH) left = !left;
        if (face == front) return m_TextureFrame + (left ? 15 : 16);
        if (face == Facing::OPPOSITE[front]) return m_TextureFrame + (left ? 32 : 31);
        return m_TextureFrame;
    }

    if (isDoubleNS)
    {
        front = Facing::EAST;
        left = id_north == m_ID;
        TilePos side = left ? pos.north() : pos.south();

        int id_behind = level->getTile(side.west());
        int id_infront = level->getTile(side.east());

        if ((Tile::solid[id_west] || Tile::solid[id_behind]) && !Tile::solid[id_east] && !Tile::solid[id_infront]) {
            front = Facing::EAST;
        }
        else if ((Tile::solid[id_east] || Tile::solid[id_infront]) && !Tile::solid[id_west] && !Tile::solid[id_behind]) {
            front = Facing::WEST;
            left = !left;
        }

        if (face == front)
            return m_TextureFrame + (left ? 15 : 16);
        if (face == Facing::OPPOSITE[front])
            return m_TextureFrame + (left ? 32 : 31);
        return m_TextureFrame;
    }

    return m_TextureFrame;
}

int ChestTile::getTexture(Facing::Name face) const
{
	switch (face) {
	case Facing::UP:
    case Facing::DOWN:
        return m_TextureFrame - 1;
	case Facing::SOUTH:
        return m_TextureFrame + 1;
	default:
        return m_TextureFrame;
	}
}

bool ChestTile::mayPlace(const Level* level, const TilePos& pos) const
{
    return !hasNeighbors(level, pos, 1);
}

bool ChestTile::hasNeighbors(const Level* level, const TilePos& pos, int count) const
{
    int neighbors = 0;
    for (int i = 0; i < 4; ++i)
    {
        Facing::Name f = static_cast<Facing::Name>(Facing::HORIZONTAL[i]);
        TilePos relative = pos.relative(f);
        if (level->getTile(relative) == m_ID)
        {
            neighbors++;
            if (neighbors > count) return true;

            if (hasNeighbors(level, relative, 0)) return true;
        }
    }

    return false;
}

void ChestTile::onRemove(Level* level, const TilePos& pos){
    ChestTileEntity* ent = static_cast<ChestTileEntity*>(level->getTileEntity(pos));

    if (!ent)
    {
        Tile::onRemove(level, pos);
    }

    for (int slot = 0; slot < ent->getContainerSize(); ++slot)
    {
        ItemStack& item = ent->getItem(slot);
        if (!item.isEmpty())
        {
            TilePos offset(
                (m_chestRandom.nextFloat() * 0.8f) + 0.1f,
                (m_chestRandom.nextFloat() * 0.8f) + 0.1f,
                (m_chestRandom.nextFloat() * 0.8f) + 0.1f
            );

            while (item.m_count > 0)
            {
                int toRemove = std::min(m_chestRandom.nextInt(21) + 10, static_cast<int>(item.m_count));
                item.m_count -= toRemove;
                ItemEntity* itemEnt = new ItemEntity(level, pos + offset, ItemStack(item.getItem()->m_itemID, toRemove, item.getAuxValue()));
                float spread = 0.05f;
                itemEnt->m_vel.x = (double)((float)m_chestRandom.nextGaussian() * spread);
                itemEnt->m_vel.y = (double)((float)m_chestRandom.nextGaussian() * spread + 0.2f);
                itemEnt->m_vel.z = (double)((float)m_chestRandom.nextGaussian() * spread);
                level->addEntity(itemEnt);
            }
        }
    }

    Tile::onRemove(level, pos);
}

bool ChestTile::use(Level* level, const TilePos& pos, Player* player) 
{
    if (level->m_bIsClientSide)
        return true;

    if (level->isSolidTile(pos.above()))
        return true;
    
    if (level->getTile(pos.west()) == m_ID && level->isSolidTile(pos + TilePos(-1, 1, 0)))
        return true;

    if (level->getTile(pos.east()) == m_ID && level->isSolidTile(pos + TilePos(1, 1, 0)))
        return true;

    if (level->getTile(pos.north()) == m_ID && level->isSolidTile(pos + TilePos(0, 1, -1)))
        return true;

    if (level->getTile(pos.south()) == m_ID && level->isSolidTile(pos + TilePos(0, 1, 1)))
        return true;

    TileEntity* tileEnt = level->getTileEntity(pos);
    if (!tileEnt)
        return false;

    ChestTileEntity* chest = static_cast<ChestTileEntity*>(tileEnt);
    Container* container = static_cast<Container*>(chest);
    for (int rel = Facing::NORTH; rel <= Facing::EAST; rel++) 
    {
        TilePos relPos = pos.relative(static_cast<Facing::Name>(rel));
        if (level->getTile(pos.relative(static_cast<Facing::Name>(rel))) == m_ID)
        {
            TileEntity* nearbyTileEntity = level->getTileEntity(relPos);
            if (!nearbyTileEntity)
                continue;

            Container* nearbyContainer = static_cast<Container*>(static_cast<ChestTileEntity*>(nearbyTileEntity));
            if (rel % 2 == 0)
                container = new CompoundContainer("Large chest", nearbyContainer, container);
            else
                container = new CompoundContainer("Large chest", container, nearbyContainer);
            break;
        }
    }

    player->openContainer(container);
    return true;
}

bool ChestTile::hasTileEntity() const
{
    return true;
}

TileEntity* ChestTile::newTileEntity()
{
    return new ChestTileEntity();
}
