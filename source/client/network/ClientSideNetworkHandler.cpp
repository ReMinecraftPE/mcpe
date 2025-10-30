/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <RakPeer.h>
#include "ClientSideNetworkHandler.hpp"
#include "common/Logger.hpp"
#include "client/gui/screens/StartMenuScreen.hpp"
#include "client/gui/screens/DisconnectionScreen.hpp"
#include "client/multiplayer/MultiPlayerLevel.hpp"
#include "client/multiplayer/MultiplayerLocalPlayer.hpp"
#include "network/MinecraftPackets.hpp"
#include "world/entity/MobFactory.hpp"

// This lets you make the client shut up and not log events in the debug console.
//#define VERBOSE_CLIENT

#if defined(ORIGINAL_CODE) || defined(VERBOSE_CLIENT)
#define puts_ignorable(str) LOG_I(str)
#define printf_ignorable(str, ...) LOG_I(str, __VA_ARGS__)
#else
#define puts_ignorable(str)
#define printf_ignorable(str, ...)
#endif

ClientSideNetworkHandler::ClientSideNetworkHandler(Minecraft* pMinecraft, RakNetInstance* pRakNetInstance)
{
	m_pMinecraft = pMinecraft;
	m_pRakNetInstance = pRakNetInstance;
	m_pServerPeer = m_pRakNetInstance->getPeer();
	m_chunksRequested = 0;
	m_chunkCount = 0;
	m_serverProtocolVersion = 0;
	m_bUseLevelDataPkt = false;
	m_pLevel = nullptr;
	m_field_14 = 0;
	m_field_24 = 0;
	clearChunksLoaded();
}

void ClientSideNetworkHandler::levelGenerated(Level* level)
{
	m_pLevel = (MultiPlayerLevel*)level;

	if (m_serverProtocolVersion >= 3)
	{
		ReadyPacket* pReadyPkt = new ReadyPacket(1);
		m_pRakNetInstance->send(pReadyPkt);
	}

	arrangeRequestChunkOrder();
	requestNextChunk();
}

void ClientSideNetworkHandler::onConnect(const RakNet::RakNetGUID& rakGuid) // server guid
{
	RakNet::RakNetGUID localGuid = ((RakNet::RakPeer*)m_pServerPeer)->GetMyGUID(); // iOS 0.2.1 crashes right here after loading chunks
	printf_ignorable("onConnect, server guid: %s, local guid: %s", rakGuid.ToString(), localGuid.ToString());

	m_serverGUID = rakGuid;

	clearChunksLoaded();
	LoginPacket* pLoginPkt = new LoginPacket(m_pMinecraft->m_pUser->field_0, NETWORK_PROTOCOL_VERSION);
	m_pRakNetInstance->send(pLoginPkt);
}

void ClientSideNetworkHandler::onUnableToConnect()
{
	puts_ignorable("onUnableToConnect");

	// get rid of the prepare-thread to stop preparation immediately
	if (m_pMinecraft->m_pPrepThread)
	{
		delete m_pMinecraft->m_pPrepThread;
		m_pMinecraft->m_pPrepThread = nullptr;
	}

	// On 0.2.1 this is handled in ProgressScreen::tick(), and this function is instead left blank, but I see no reason to do the same.
	m_pMinecraft->setScreen(new DisconnectionScreen("Could not connect to server. Try again."));
}

void ClientSideNetworkHandler::onDisconnect(const RakNet::RakNetGUID& rakGuid)
{
	puts_ignorable("onDisconnect");

	if (m_pLevel)
		m_pLevel->m_bIsClientSide = false;

	m_pMinecraft->m_gui.addMessage("Disconnected from server");
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, LoginStatusPacket* pPacket)
{
	puts_ignorable("LoginStatusPacket");

	switch (pPacket->m_loginStatus)
	{
	case LoginStatusPacket::STATUS_CLIENT_OUTDATED:
		m_pMinecraft->setScreen(new DisconnectionScreen("Could not connect: Outdated client!"));
		break;
	case LoginStatusPacket::STATUS_SERVER_OUTDATED:
		m_pMinecraft->setScreen(new DisconnectionScreen("Could not connect: Outdated server!"));
		break;
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, MessagePacket* pMsgPkt)
{
	puts_ignorable("MessagePacket");

	m_pMinecraft->m_gui.addMessage(pMsgPkt->m_str.C_String());
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, SetTimePacket* pPacket)
{
	//puts_ignorable("SetTimePacket");

	if (m_pLevel)
		m_pLevel->setTime(pPacket->m_time);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, StartGamePacket* pStartGamePkt)
{
	puts_ignorable("StartGamePacket");

	std::string levelName = "_LastJoinedServer";
	m_pMinecraft->getLevelSource()->deleteLevel(levelName);

	LevelSettings settings(pStartGamePkt->m_seed, pStartGamePkt->m_gameType);
	m_pLevel = new MultiPlayerLevel(
		m_pMinecraft->getLevelSource()->selectLevel(levelName, true),
		"temp",
		settings,
		pStartGamePkt->m_levelVersion);

	m_pLevel->m_bIsClientSide = true;

	MultiplayerLocalPlayer *pLocalPlayer = new MultiplayerLocalPlayer(m_pMinecraft, m_pLevel, m_pMinecraft->m_pUser, settings.m_gameType, m_pLevel->m_pDimension->field_50);
	pLocalPlayer->m_guid = ((RakNet::RakPeer*)m_pServerPeer)->GetMyGUID();
	pLocalPlayer->m_EntityID = pStartGamePkt->m_entityId;
	
	pLocalPlayer->moveTo(
		pStartGamePkt->m_pos,
		pLocalPlayer->m_rot);

	if (settings.m_gameType == GAME_TYPE_CREATIVE)
		pLocalPlayer->m_pInventory->prepareCreativeInventory();
	else
		pLocalPlayer->m_pInventory->prepareSurvivalInventory();

	m_pLevel->setTime(pStartGamePkt->m_time);

	// Not replicated even by 0.12.1, not sure where we even got this from...
	m_serverProtocolVersion = NETWORK_PROTOCOL_VERSION;

	m_pMinecraft->setLevel(m_pLevel, "ClientSideNetworkHandler -> setLevel", pLocalPlayer);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, AddPlayerPacket* pAddPlayerPkt)
{
	puts_ignorable("AddPlayerPacket");

	if (!m_pLevel) return;

	Player* pPlayer = new Player(m_pLevel, m_pLevel->getDefaultGameType());
	pPlayer->m_EntityID = pAddPlayerPkt->m_id;
	m_pLevel->addEntity(pPlayer);

	// If we haven't received a rot, use default player rot
	if (pAddPlayerPkt->m_rot == Vec2::ZERO)
		pAddPlayerPkt->m_rot = pPlayer->m_rot;

	pPlayer->moveTo(
		pAddPlayerPkt->m_pos,
		pAddPlayerPkt->m_rot);

	pPlayer->m_name = pAddPlayerPkt->m_name;
	pPlayer->m_guid = pAddPlayerPkt->m_guid;

	if (pPlayer->getPlayerGameType() == GAME_TYPE_CREATIVE)
		pPlayer->m_pInventory->prepareCreativeInventory();
	else
		pPlayer->m_pInventory->prepareSurvivalInventory();

	ItemInstance* pItem = pPlayer->getSelectedItem();
	if (pItem)
	{
		*pItem = ItemInstance(pAddPlayerPkt->m_itemId, pAddPlayerPkt->m_itemAuxValue, 63);
	}

	m_pMinecraft->m_gui.addMessage(pPlayer->m_name + " joined the game");
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, AddMobPacket* pAddMobPkt)
{
	//puts_ignorable("AddMobPacket");

	if (!m_pLevel)
	{
		LOG_W("Trying to add a mob with no level!");
		return;
	}

	EntityType::ID entityTypeId = (EntityType::ID)pAddMobPkt->m_entityTypeId;
	if (entityTypeId == EntityType::UNKNOWN)
	{
		LOG_E("Trying to add a mob without a type id");
		return;
	}

	Entity* entity = MobFactory::CreateMob(entityTypeId, m_pLevel);
	// Mojang, in all of their infinite wisdon, does not have this check here in 0.2.1,
	// so the game will just crash if you replicate a mob it can't create.
	if (!entity)
	{
		LOG_E("Server tried to add an unknown mob type! :%d", entityTypeId);
		return;
	}

	entity->m_EntityID = pAddMobPkt->m_entityId;
	entity->moveTo(pAddMobPkt->m_pos, pAddMobPkt->m_rot);
	entity->getEntityData().assignValues(pAddMobPkt->getUnpackedData());
	m_pLevel->addEntity(entity);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, RemoveEntityPacket* pRemoveEntityPkt)
{
	if (!m_pLevel) return;

	Entity* pEnt = m_pLevel->getEntity(pRemoveEntityPkt->m_id);

	if (pEnt)
		m_pLevel->removeEntity(pEnt);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, AddItemEntityPacket* packet)
{
	puts_ignorable("AddItemEntityPacket");

	if (!m_pLevel) return;

	ItemInstance* pItemInstance = new ItemInstance(packet->m_itemId, packet->m_itemCount, packet->m_auxValue);
	if (pItemInstance->isNull())
	{
		delete pItemInstance;
		LOG_E("Received invalid or null ItemInstance from server!");
		return;
	}

	ItemEntity* pItemEntity = new ItemEntity(m_pLevel, packet->m_pos, pItemInstance);

	pItemEntity->m_vel.x = packet->m_velX * (1.f / 128.f);
	pItemEntity->m_vel.y = packet->m_velY * (1.f / 128.f);
	pItemEntity->m_vel.z = packet->m_velZ * (1.f / 128.f);

	m_pLevel->putEntity(packet->m_entityId, pItemEntity);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, TakeItemEntityPacket* pkt)
{
	puts_ignorable("TakeItemEntityPacket");

	if (!m_pLevel) return;

	Entity* pEntity = m_pLevel->getEntity(pkt->m_targetId);
	if (!pEntity)
	{
		LOG_E("Failed to handle TakeItemEntityPacket: Unknown ItemEntity: %d", pkt->m_targetId);
		return;
	}

	if (!pEntity->getDescriptor().isType(EntityType::ITEM))
		return;
	ItemEntity* pItemEntity = (ItemEntity*)pEntity;

	if (m_pMinecraft->m_pLocalPlayer->m_EntityID == pkt->m_sourceId)
	{
		if (pItemEntity->m_pItemInstance)
		{
			if (m_pMinecraft->m_pLocalPlayer->m_pInventory->addItem(*pItemEntity->m_pItemInstance))
			{
				m_pLevel->playSound(pItemEntity, "random.pop", 0.3f,
					((Entity::sharedRandom.nextFloat() - Entity::sharedRandom.nextFloat()) * 0.7f + 1.0f) * 2.0f);
			}
		}
	}
}	

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, MoveEntityPacket* packet)
{
	if (!m_pLevel) return;

	Entity* pEntity = m_pLevel->getEntity(packet->m_entityId);
	if (!pEntity) return;

	Vec2 rot;
	if (packet->m_bHasRot)
	{
		rot = packet->m_rot;
	}
	else
	{
		rot = pEntity->m_rot;
	}
	
	pEntity->lerpTo(packet->m_pos, rot);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, MovePlayerPacket* packet)
{
	if (!m_pLevel) return;

	Entity* pEntity = m_pLevel->getEntity(packet->m_id);
	if (!pEntity)
	{
		LOG_E("MovePlayerPacket: No player with id %d", packet->m_id);
		return;
	}

	pEntity->lerpTo(packet->m_pos, packet->m_rot);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, PlaceBlockPacket* pPlaceBlockPkt)
{
	puts_ignorable("PlaceBlockPacket");

	Player* pPlayer = (Player*)m_pLevel->getEntity(pPlaceBlockPkt->m_entityId);
	if (!pPlayer)
	{
		LOG_E("PlaceBlockPacket: No player with id %d", pPlaceBlockPkt->m_entityId);
		return;
	}

	pPlayer->swing();

	// @BUG: Not buffering the update.
	if (!areAllChunksLoaded())
		return;

	const TilePos& pos = pPlaceBlockPkt->m_pos;
	TileID tileTypeId = pPlaceBlockPkt->m_tileTypeId;
	Facing::Name face = (Facing::Name)pPlaceBlockPkt->m_face;

	if (!m_pLevel->mayPlace(tileTypeId, pos, true))
		return;

	Tile* pTile = Tile::tiles[tileTypeId];
	if (!m_pLevel->setTile(pos, tileTypeId))
		return;

	pTile->setPlacedOnFace(m_pLevel, pos, face);
	pTile->setPlacedBy(m_pLevel, pos, pPlayer);

	const Tile::SoundType* pSound = pTile->m_pSound;
	m_pLevel->playSound(pos + 0.5f, "step." + pSound->m_name, 0.5f * (1.0f + pSound->volume), 0.8f * pSound->pitch);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, RemoveBlockPacket* pRemoveBlockPkt)
{
	puts_ignorable("RemoveBlockPacket");

	Entity* pEntity = m_pLevel->getEntity(pRemoveBlockPkt->m_entityId);
	if (!pEntity || !pEntity->isPlayer())
	{
		LOG_E("RemoveBlockPacket: No player with id %d", pRemoveBlockPkt->m_entityId);
		return;
	}

	Player* pPlayer = (Player*)pEntity;

	pPlayer->swing();

	// @BUG: Not buffering the update.
	if (!areAllChunksLoaded())
		return;

	const TilePos& pos = pRemoveBlockPkt->m_pos;
	Tile* pTile = Tile::tiles[m_pLevel->getTile(pos)];
	int auxValue = m_pLevel->getData(pos);

	m_pMinecraft->m_pParticleEngine->destroyEffect(pos);

	bool setTileResult = m_pLevel->setTile(pos, TILE_AIR);
	if (pTile && setTileResult)
	{
		const Tile::SoundType* pSound = pTile->m_pSound;
		m_pLevel->playSound(pos + 0.5f, "step." + pSound->m_name, 0.5f * (1.0f + pSound->volume), 0.8f * pSound->pitch);

		pTile->destroy(m_pLevel, pos, auxValue);
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, UpdateBlockPacket* pkt)
{
	BlockUpdate update(pkt->m_pos, pkt->m_tileTypeId, pkt->m_data);

	if (!areAllChunksLoaded())
	{
		m_bufferedBlockUpdates.push_back(update);
		return;
	}

	handleBlockUpdate(update);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, LevelEventPacket* pkt)
{
	//puts_ignorable("LevelEventPacket");
	if (!m_pLevel) return;

	m_pLevel->levelEvent(nullptr, pkt->m_eventId, pkt->m_pos, pkt->m_data);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, EntityEventPacket* pkt)
{
	//puts_ignorable("EntityEventPacket");
	if (!m_pLevel) return;

	Entity* pEntity = m_pLevel->getEntity(pkt->m_entityId);
	if (pEntity)
		pEntity->handleEntityEvent(pkt->m_eventId);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, ChunkDataPacket* pChunkDataPkt)
{
	if (!m_pLevel)
	{
		LOG_E("Level @ handle ChunkDataPacket is 0");
		return;
	}

	LevelChunk* pChunk = m_pLevel->getChunkSource()->create(pChunkDataPkt->m_chunkPos);
	if (!pChunk || pChunk->isEmpty())
	{
		LOG_E("Failed to find write-able chunk");
		// @BUG: Not trying again.
		return;
	}

	int x16 = 16 * pChunkDataPkt->m_chunkPos.x;
	int z16 = 16 * pChunkDataPkt->m_chunkPos.z;

	bool updated = false;

	int minY = 128, maxY = 0;
	int minX = 16,  minZ = 16;
	int maxX = 0,   maxZ = 0;

	for (int k = 0; k < C_MAX_CHUNKS; k++)
	{
		uint8_t updMap;
		pChunkDataPkt->m_data.Read(updMap);

		if (!updMap)
			continue;

		for (int j = 0; j < 8; j++)
		{
			if ((updMap >> j) & 1)
			{
				int yPos = j * 16;
				TileID  tiles[16];
				uint8_t datas[16 / 2];

				pChunkDataPkt->m_data.Read((char*)tiles, 16 * sizeof(TileID));
				pChunkDataPkt->m_data.Read((char*)datas, 16 / 2);

				for (int i = 0; i < 16; i++)
				{
					m_pLevel->setTileNoUpdate(TilePos(x16 + (k & 0xF), yPos + i, z16 + (k >> 4)), tiles[i]);
				}

				int idx = ((k & 0xF) << 11) | ((k >> 4) << 7) + yPos;
				memcpy(&pChunk->m_tileData.m_data[idx >> 1], datas, sizeof datas);
			}

			int ymin = 16 * (1 << j);
			if (minY >= ymin)
				minY = ymin;
			if (maxY < ymin + 15)
				maxY = ymin + 15;
		}

		if (minX >= (k & 0xF))
			minX = k & 0xF;
		if (minZ >= (k >> 4))
			minZ = k >> 4;
		if (maxX <= (k & 0xF))
			maxX = k & 0xF;
		if (maxZ <= (k >> 4))
			maxZ = k >> 4;

		updated = true;
	}

	if (updated)
		m_pLevel->setTilesDirty(TilePos(minX + x16, minY, minZ), TilePos(maxX + x16, maxY, maxZ + z16));

	pChunk->m_bUnsaved = true;
	m_chunkStates[pChunkDataPkt->m_chunkPos.x][pChunkDataPkt->m_chunkPos.z] = true;
	if (!m_bUseLevelDataPkt)
	{
		if (areAllChunksLoaded())
			flushAllBufferedUpdates();
		else
			requestNextChunk();
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, PlayerEquipmentPacket* pPlayerEquipmentPkt)
{
	if (!m_pLevel)
		return;

	Player* pPlayer = (Player*)m_pLevel->getEntity(pPlayerEquipmentPkt->m_playerID);
	if (!pPlayer)
		return;

	if (pPlayer->m_guid == m_pServerPeer->GetMyGUID())
	{
		LOG_W("Attempted to modify local player's inventory");
		return;
	}

	pPlayer->m_pInventory->selectItemById(pPlayerEquipmentPkt->m_itemID, C_MAX_HOTBAR_ITEMS);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, InteractPacket* pkt)
{
	//puts_ignorable("InteractPacket");
	if (!m_pLevel) return;

	Entity* pSource = m_pLevel->getEntity(pkt->m_sourceId);
	Entity* pTarget = m_pLevel->getEntity(pkt->m_targetId);
	if (!pSource || !pTarget)
		return;

	if (!pSource->isPlayer())
		return;

	Player* pPlayer = (Player*)pSource;
	switch (pkt->m_actionType)
	{
	case InteractPacket::INTERACT:
		pPlayer->swing();
		m_pMinecraft->m_pGameMode->interact(pPlayer, pTarget);
		break;
	case InteractPacket::ATTACK:
		pPlayer->swing();
		m_pMinecraft->m_pGameMode->attack(pPlayer, pTarget);
		break;
	default:
		LOG_W("Received unkown action in InteractPacket: %d", pkt->m_actionType);
		break;
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, SetEntityDataPacket* pkt)
{
	puts_ignorable("SetEntityDataPacket");

	if (!m_pLevel)
		return;

	Entity* pEntity = m_pLevel->getEntity(pkt->m_entityId);
	if (!pEntity)
		return;

	pEntity->getEntityData().assignValues(pkt->getUnpackedData());
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, SetHealthPacket* pkt)
{
	puts_ignorable("SetHealthPacket");

	if (!m_pLevel)
		return;

	LocalPlayer* pLocalPlayer = m_pMinecraft->m_pLocalPlayer;
	if (pLocalPlayer)
		pLocalPlayer->hurtTo(pkt->m_health);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, AnimatePacket* pkt)
{
	//puts_ignorable("AnimatePacket");

	if (!m_pLevel)
		return;

	Entity* pEntity = m_pLevel->getEntity(pkt->m_entityId);
	if (!pEntity)
		return;

	switch (pkt->m_actionId)
	{
		case AnimatePacket::SWING:
		{
			if (!pEntity->isMob())
				break;
			Mob* pMob = (Mob*)pEntity;

			pMob->swing();
			break;
		}
		case AnimatePacket::HURT:
		{
			pEntity->animateHurt();
			break;
		}
		default:
		{
			LOG_W("Received unkown action in AnimatePacket: %d, EntityType: %s", pkt->m_actionId, pEntity->getDescriptor().getEntityType().getName().c_str());
			break;
		}
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, RespawnPacket* packet)
{
	puts_ignorable("RespawnPacket");

	if (!m_pLevel)
		return;

	NetEventCallback::handle(*m_pLevel, guid, packet);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, LevelDataPacket* packet)
{
	const int uncompMagic = 12847812, compMagic = 58712758, chunkSepMagic = 284787658;
	RakNet::BitStream* bs = &packet->m_data, bs2;

	int magicNum = 0;
	bs->Read(magicNum);
	if (magicNum != compMagic && magicNum != uncompMagic)
	{
		LOG_E("Invalid level data packet with magic %d", magicNum);
		return;
	}

	// If our data is compressed
	if (magicNum == compMagic)
	{
		// Decompress it before we handle it.
		int uncompSize = 0, compSize = 0;
		bs->Read(uncompSize);
		bs->Read(compSize);

		LOG_I("Decompressing level data. Compressed: %d bytes, uncompressed: %d bytes", compSize, uncompSize);

		// Read the compressed data.
		uint8_t* pCompData = new uint8_t[compSize];
		bs->Read((char*)pCompData, compSize);

		// Inflate it.
		uint8_t* pUncompData = ZlibInflateToMemory(pCompData, compSize, uncompSize);
		SAFE_DELETE_ARRAY(pCompData);

		// If we couldn't, bail
		if (!pUncompData)
		{
			LOG_E("Failed to decompress level data!");
			return;
		}

		// Do some small scale hacks to get bs2 contain the uncompressed data.
		bs2.Reset();
		bs2.Write((const char*)pUncompData, uncompSize);
		bs2.ResetReadPointer();
		bs = &bs2;

		// Delete the uncompressed data, since we've written it to our bitstream.
		SAFE_DELETE_ARRAY(pUncompData);

		bs->Read(magicNum);
	}

	int chunksX = 0, chunksZ = 0;
	bs->Read(chunksX);
	bs->Read(chunksZ);

	if (chunksX != C_MAX_CHUNKS_X || chunksZ != C_MAX_CHUNKS_Z)
	{
		LOG_E("We don't yet support a level of size %d x %d chunks. Some chunks may disappear or be regenerated.", chunksX, chunksZ);
	}

	ChunkPos cp(0, 0);
	for (cp.x = 0; cp.x < chunksX; cp.x++)
	{
		for (cp.z = 0; cp.z < chunksZ; cp.z++)
		{
			bs->Read(magicNum);

			if (magicNum != chunkSepMagic)
			{
			_FAIL_BECAUSE_INVALID:
				LOG_E("Aborting because level data is invalid, reading chunk %d, %d. Magic: %d", cp.x, cp.z, magicNum);
				return;
			}
			
			uint8_t ptype = 0;

			// read the data size. This'll let us know how much to read.
			int dataSize = 0;
			bs->Read(dataSize);

			LevelChunk* pChunk = m_pLevel->getChunk(cp);
			if (!pChunk || pChunk->isEmpty())
				LOG_E("No chunk at %d, %d", cp.x, cp.z);
			
			// continue reading anyway to skip over the offending chunk

			// Seems like reading a bitstream from another bitstream reads all the way
			// to the end, so we need to duplicate in this fashion.
			RakNet::BitStream bs2;
			bs2.Write(*bs, 8 * dataSize);
			
			// Ensure the packet type is correct.
			bs2.Read(ptype);
			if (ptype != PACKET_CHUNK_DATA)
				goto _FAIL_BECAUSE_INVALID;

			// Read the chunk data packet itself, and handle it.
			ChunkDataPacket cdp(cp, pChunk);
			cdp.read(bs2);

			if (pChunk)
				handle(guid, &cdp);

			// Handle lighting immediately, to ensure it doesn't get out of control.
			while (m_pLevel->updateLights());
		}
	}

	// All chunks are loaded. Also flush all the updates we've buffered.
	m_chunksRequested = C_MAX_CHUNKS;
	flushAllBufferedUpdates();
}

bool ClientSideNetworkHandler::areAllChunksLoaded()
{
	return m_chunksRequested > C_MAX_CHUNKS;
}

bool ClientSideNetworkHandler::isChunkLoaded(const ChunkPos& cp)
{
	if ((cp.x < 0 || cp.x >= C_MAX_CHUNKS_X) || (cp.z < 0 || cp.z >= C_MAX_CHUNKS_Z))
		return true;

	return m_chunkStates[cp.x][cp.z];
}

struct _ChunkSorter
{
	ChunkPos m_pos;

	_ChunkSorter(const ChunkPos& pos)
	{
		m_pos = pos;
	}

	bool operator()(const ChunkPos& a, const ChunkPos& b) const
	{
		return (a - m_pos).lengthSqr() < (b - m_pos).lengthSqr();
	}
};

void ClientSideNetworkHandler::arrangeRequestChunkOrder()
{
	clearChunksLoaded();

	ChunkPos cp(C_MAX_CHUNKS_X / 2, C_MAX_CHUNKS_Z / 2);

	if (m_pMinecraft)
	{
		LocalPlayer* pLocalPlayer = m_pMinecraft->m_pLocalPlayer;
		if (pLocalPlayer)
		{
			cp = pLocalPlayer->m_pos;
		}
	}

	std::sort(&m_orderedChunks[0], &m_orderedChunks[C_MAX_CHUNKS-1], _ChunkSorter(cp));
}

void ClientSideNetworkHandler::clearChunksLoaded()
{
	for (int i = 0; i < C_MAX_CHUNKS; i++)
	{
		ChunkPos cp(i >> 4, i & 0xF);

		m_orderedChunks[i] = cp;
		m_chunkStates[cp.x][cp.z] = false;
	}
}

void ClientSideNetworkHandler::requestNextChunk()
{
	if (areAllChunksLoaded())
		return;

	if (m_serverProtocolVersion < 2)
	{
		m_pRakNetInstance->send(new RequestChunkPacket(ChunkPos(m_chunksRequested % 16, m_chunksRequested / 16)));
		m_chunksRequested++;
	}
	else if (m_bUseLevelDataPkt)
	{
		m_pRakNetInstance->send(new RequestChunkPacket(ChunkPos(-9999, -9999)));
	}
	else
	{
		m_pRakNetInstance->send(new RequestChunkPacket(m_orderedChunks[m_chunkCount]));
		m_chunkCount++;
		m_chunksRequested++;
	}
}

void ClientSideNetworkHandler::flushAllBufferedUpdates()
{
	for (int i = 0; i < int(m_bufferedBlockUpdates.size()); i++)
	{
		handleBlockUpdate(m_bufferedBlockUpdates[i]);
	}
}

void ClientSideNetworkHandler::handleBlockUpdate(const BlockUpdate& u)
{
	m_pLevel->setTileAndData(u.pos, Tile::TransformToValidBlockId(u.tile), u.data);
}