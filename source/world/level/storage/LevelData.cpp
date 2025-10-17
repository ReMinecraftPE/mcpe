/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LevelData.hpp"
#include "network/RakIO.hpp"
#include "nbt/NbtIo.hpp"

#define FORCE_SURVIVAL_MODE (TEST_SURVIVAL_MODE || 0)

void LevelData::_init()
{
	_init(LevelSettings());
}

void LevelData::_init(const LevelSettings& settings, int storageVersion)
{
	m_levelName = std::string();
	m_seed = settings.m_seed;
	m_spawnPos = TilePos(128, 64, 128);
	m_time = 0;
	m_lastPlayed = 0;
	m_sizeOnDisk = 0;
	m_playerTag = nullptr;
	m_dimensionId = 0;
	m_gameType = settings.m_gameType;
	m_storageVersion = storageVersion;
	m_generatorVersion = 0; // pre-0.2.1 versions used storageVersion instead
	m_bSpawnMobs = false;
	m_nPlayers = -1;
}

void LevelData::_init(const LevelSettings& settings, int storageVersion, const std::string& name)
{
	_init(settings, storageVersion);
	m_levelName = name;
}

LevelData::~LevelData()
{
	setPlayerTag(nullptr);
}

void LevelData::v1_read(RakNet::BitStream& bs, int storageVersion)
{
	setStorageVersion(storageVersion);
	bs.Read(m_seed);
	bs.Read(m_spawnPos);
	bs.Read(m_time);
	bs.Read(m_sizeOnDisk);
	bs.Read(m_lastPlayed);

	RakNet::RakString rs;
	bs.Read(rs);
	m_levelName = std::string(rs.C_String());
}

void LevelData::v1_write(RakNet::BitStream& bs) const
{
	bs.Write(m_seed);
	bs.Write(m_spawnPos);
	bs.Write(m_time);
	bs.Write(m_sizeOnDisk);
	bs.Write(int(getEpochTimeS()));

	RakNet::RakString rs(m_levelName.c_str());
	bs.Write(rs);
}

void LevelData::read(RakNet::BitStream& bs, int storageVersion)
{
	// Actual PE doesn't call setStorageVersion() at all.
	// It did not respect the storageVersion of the Level after LevelData::read() had been called.
	// Instead, it relied on LevelData's default storageVersion. This was yet another way of forcing world save upgrades.
	setStorageVersion(storageVersion);

	RakDataInput dis = RakDataInput(bs);
	CompoundTag* tag = NbtIo::read(dis);

	if (tag)
	{
		if (tag->getId() == Tag::TAG_TYPE_COMPOUND)
		{
			loadTagData(*tag);
		}

		tag->deleteChildren();
		delete tag;
	}
}

void LevelData::write(RakNet::BitStream& bs, const std::vector<Player*>* players) const
{
	RakDataOutput dos(bs);
	CompoundTag tag;

	if (!players || players->empty())
		tag = createTag();
	else
		tag = createTag(*players);

	NbtIo::write(tag, dos);

	tag.deleteChildren();
}

CompoundTag LevelData::createTag() const
{
	CompoundTag levelTag;
	CompoundTag* playerTag = nullptr;

	if (m_playerTag)
    {
		playerTag = m_playerTag->copy();
        // Don't want to accidentally de-allocate the current playerTag
        playerTag->leak();
    }

	writeTagData(levelTag, playerTag);

	return levelTag;
}

CompoundTag LevelData::createTag(const std::vector<Player*>& players) const
{
	CompoundTag levelTag;
	CompoundTag* playerTag = nullptr;
	
	if (!players.empty())
	{
		playerTag = new CompoundTag();
		players[0]->saveWithoutId(*playerTag);
	}

	writeTagData(levelTag, playerTag);

	return levelTag;
}

void LevelData::loadTagData(CompoundTag& tag)
{
    setSeed(tag.getInt64("RandomSeed"));
    setGameType((GameType)tag.getInt32("GameType"));

    setXSpawn(tag.getInt32("SpawnX"));
    setYSpawn(tag.getInt32("SpawnY"));
    setZSpawn(tag.getInt32("SpawnZ"));

    setTime(tag.getInt64("Time"));
    _setLastPlayed(tag.getInt64("LastPlayed"));
    setSizeOnDisk(tag.getInt64("SizeOnDisk"));
    setLevelName(tag.getString("LevelName"));
	setStorageVersion(tag.getInt32("StorageVersion"));

	setSpawnMobs(getGameType() == GAME_TYPE_SURVIVAL);

	CompoundTag* playerTag = tag.getCompound("Player");
    if (playerTag)
    {
        setPlayerTag(playerTag);
        // "Transfer ownership" of tag contents to playerTag copy
        playerTag->leak();
    }
}

void LevelData::writeTagData(CompoundTag& levelTag, CompoundTag* playerTag) const
{
	CompoundTag& tag = levelTag;

	tag.putInt64("RandomSeed", getSeed());
	tag.putInt32("GameType", getGameType());

	tag.putInt32("SpawnX", getXSpawn());
	tag.putInt32("SpawnY", getYSpawn());
	tag.putInt32("SpawnZ", getZSpawn());

	tag.putInt64("Time", getTime());
	tag.putInt64("SizeOnDisk", getSizeOnDisk());
	tag.putInt64("LastPlayed", getEpochTimeS());
	tag.putString("LevelName", getLevelName());
	tag.putInt32("StorageVersion", getStorageVersion());
	tag.putInt32("Platform", 2); // what's 2? why is it only written and not read? not sure...

	if (playerTag)
	{
		tag.putCompound("Player", playerTag);
	}
}

void LevelData::setLoadedPlayerTag(CompoundTag* playerTag)
{
	if (m_playerTag == playerTag)
		return;

	if (m_playerTag)
	{
		m_playerTag->deleteChildren();
		delete m_playerTag;
	}

	m_playerTag = playerTag;
}

void LevelData::setPlayerTag(const CompoundTag* playerTag)
{
	if (m_playerTag == playerTag)
		return;

	if (m_playerTag)
	{
		m_playerTag->deleteChildren();
		delete m_playerTag;
		m_playerTag = nullptr;
	}

	if (playerTag)
		m_playerTag = playerTag->copy();
}

GameType LevelData::getGameType() const
{
#if FORCE_SURVIVAL_MODE
	return GAME_TYPE_SURVIVAL;
#else
	return m_gameType;
#endif
}

void LevelData::setLoadedPlayerTo(Player& player) const
{
	if (m_nPlayers == 1)
		m_LocalPlayerData.loadPlayer(player);
}

void PlayerData::loadPlayer(Player& player) const
{
	player.setPos(Vec3::ZERO);

	player.m_pos     = m_pos;
	player.m_oPos    = m_pos;
	player.m_posPrev = m_pos;
	player.m_vel.x   = Mth::abs(m_vel.x) > 10.0f ? 0.0f : m_vel.x;
	player.m_vel.y   = Mth::abs(m_vel.y) > 10.0f ? 0.0f : m_vel.y;
	player.m_vel.z   = Mth::abs(m_vel.z) > 10.0f ? 0.0f : m_vel.z;

	// Let the setter do the validation
	player.setRot(
		player.m_oRot = m_rot
	);
	player.m_distanceFallen = m_distanceFallen;
	player.m_fireTicks = field_24;
	player.m_airCapacity = field_26;
	player.m_bOnGround = field_28;

	// @NOTE: Why are we updating m_pos, m_oPos and m_posPrev above if we do this?
	player.setPos(m_pos);

	// TODO: survival mode stuff
	for (int i = 0; i < C_MAX_HOTBAR_ITEMS; i++)
		player.m_pInventory->setQuickSlotIndexByItemId(i, m_hotbar[i]);
}

void PlayerData::savePlayer(const Player& player)
{
	m_pos = player.m_pos;
	m_vel = player.m_vel;
	m_rot = player.m_rot;
	m_distanceFallen = player.m_distanceFallen;
	field_24 = player.m_fireTicks;
	field_26 = player.m_airCapacity;
	field_28 = player.m_bOnGround;

	// TODO: survival mode stuff
	for (int i = 0; i < C_MAX_HOTBAR_ITEMS; i++)
		m_hotbar[i] = player.m_pInventory->getQuickSlotItemId(i);
}
