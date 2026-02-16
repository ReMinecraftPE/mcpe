#include "FurnaceTile.hpp"
#include "world/level/Level.hpp"
#include "entity/FurnaceTileEntity.hpp"

bool FurnaceTile::keepInventory = false;

FurnaceTile::FurnaceTile(int id, bool active) : EntityTile(id, TEXTURE_FURNACE_SIDE, Material::stone)
{
    setTicking(true);
	m_active = active;
}

int FurnaceTile::getTexture(const LevelSource* level, const TilePos& pos, Facing::Name face) const
{
	switch (face)
    {
	case Facing::UP:
    case Facing::DOWN:
        return m_TextureFrame + 17;
	default:
    {
		int meta = level->getData(pos);
		return face != meta ? m_TextureFrame : (m_active ? m_TextureFrame + 16 : m_TextureFrame - 1);
    }
	}
}

void FurnaceTile::animateTick(Level* level, const TilePos& pos, Random* random)
{
	if (!m_active)
        return;

    int data = level->getData(pos);
    Vec3 particlePos(pos.x + 0.5f, pos.y + 0.0f + random->nextFloat() * 6.0f / 16.0f, pos.z + 0.5f);
    const float outward = 0.52f;
    float randomOffset = random->nextFloat() * 0.6f - 0.3f;

    if (random->nextFloat() < 0.1f)
		level->playSound(Vec3(particlePos.x + 0.5f, particlePos.y + 0.5f, particlePos.z + 0.5f), "fire.fire_crackle", 1.0f, 1.0f);

    if (data == 4 || data == 5)
    {
        particlePos.x += data == 4 ? -outward : outward;
        particlePos.z += randomOffset;
    }
    else if (data == 2 || data == 3)
    {
        particlePos.x += randomOffset;
        particlePos.z += data == 2 ? -outward : outward;
    }

    level->addParticle("smoke", particlePos);
    level->addParticle("flame", particlePos);
}

int FurnaceTile::getTexture(Facing::Name face) const
{
	switch (face)
    {
	case Facing::UP:
    case Facing::DOWN:
        return m_TextureFrame + 17;
	case Facing::SOUTH:
        return (m_active) ? m_TextureFrame + 16 : m_TextureFrame - 1;
	default:
        return m_TextureFrame;
	}
}

void FurnaceTile::onPlace(Level* level, const TilePos& pos)
{
	EntityTile::onPlace(level, pos);
	RecalculateLookDirection(level, pos);
}

bool FurnaceTile::use(Level* level, const TilePos& pos, Player* player)
{
	if (player->isSneaking() && !player->getSelectedItem().isEmpty())
		return false;

	if (level->m_bIsClientSide)
		return true;

    player->openFurnace(static_cast<FurnaceTileEntity*>(level->getTileEntity(pos)));
    return true;
}

void FurnaceTile::setPlacedBy(Level* level, const TilePos& pos, Mob* mob)
{
	int rot = Mth::floor(0.5f + (mob->m_rot.x * 4.0f / 360.0f)) & 3;
	int data = 4;

	switch (rot)
	{
	case 0: data = 2; break;
	case 1: data = 5; break;
	case 2: data = 3; break;
	case 3: data = 4; break;
	}

	level->setData(pos, data);
}

void FurnaceTile::onRemove(Level* level, const TilePos& pos)
{
	if (keepInventory)
        return;

    TileEntity* tileEnt = level->getTileEntity(pos);
    
    if (!tileEnt)
    {
        EntityTile::onRemove(level, pos);
        return; // this has to be wrapped in a guard or the compiler screams, thanks -Wmisleading-indentation
    }

	FurnaceTileEntity* furnace = static_cast<FurnaceTileEntity*>(tileEnt);
	for (int i = 0; i < furnace->getContainerSize(); ++i)
    {
		ItemStack& item = furnace->getItem(i);
		if (item.isEmpty())
            continue;

        TilePos splitSource(
            m_random.nextFloat() * 0.8f + 0.1f,
            m_random.nextFloat() * 0.8f + 0.1f,
            m_random.nextFloat() * 0.8f + 0.1f
        );

        while (item.m_count > 0)
        {
            int splitCount = std::min(m_random.nextInt(21) + 10, static_cast<int>(item.m_count));
            item.m_count -= splitCount;
            ItemEntity* itemEnt = new ItemEntity(level, splitSource + pos, ItemStack(item.getId(), splitCount, item.getAuxValue()));
            
            float deviation = 0.05f;
            itemEnt->m_vel.x = (double)((float)m_random.nextGaussian() * deviation);
            itemEnt->m_vel.y = (double)((float)m_random.nextGaussian() * deviation + 0.2f);
            itemEnt->m_vel.z = (double)((float)m_random.nextGaussian() * deviation);
            level->addEntity(itemEnt);
        }
	}

    EntityTile::onRemove(level, pos);
}

void FurnaceTile::SetLit(bool lit, Level* level, const TilePos& pos)
{
    TileEntity* tileEntity = level->getTileEntity(pos);
    if (!tileEntity)
        return;

	int data = level->getData(pos);

	keepInventory = true;
    level->setTile(pos, (lit) ? Tile::furnaceLit->m_ID : Tile::furnace->m_ID);
	keepInventory = false;

	level->setData(pos, data);
	tileEntity->clearRemoved();
	level->setTileEntity(pos, tileEntity);
}

bool FurnaceTile::hasTileEntity() const
{
	return true;
}

TileEntity* FurnaceTile::newTileEntity()
{
	return new FurnaceTileEntity();
}

int FurnaceTile::getResource(TileData, Random*) const
{
	return Tile::furnace->m_ID;
}

void FurnaceTile::RecalculateLookDirection(Level* level, const TilePos& pos)
{
	TileID n = level->getTile(pos.north());
	TileID s = level->getTile(pos.south());
	TileID w = level->getTile(pos.west());
	TileID e = level->getTile(pos.east());
	uint8_t lookData = 3;

	if (Tile::solid[e] && !Tile::solid[w])
		lookData = 4;
    else if (Tile::solid[w] && !Tile::solid[e])
		lookData = 5;
    else if (Tile::solid[s] && !Tile::solid[n])
		lookData = 2;
    else if (Tile::solid[n] && !Tile::solid[s])
		lookData = 3;

	level->setData(pos, lookData);
}
