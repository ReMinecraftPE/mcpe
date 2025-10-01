/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <RakPeer.h>
#include "ClientSideNetworkHandler.hpp"
#include "common/Utils.hpp"
#include "client/gui/screens/StartMenuScreen.hpp"
#include "client/gui/screens/DisconnectionScreen.hpp"

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
	m_serverProtocolVersion = 0;
	m_pLevel = nullptr;
	m_field_14 = 0;
	m_field_24 = 0;
}

void ClientSideNetworkHandler::levelGenerated(Level* level)
{
	m_pLevel = level;

#if NETWORK_PROTOCOL_VERSION >= 3
	ReadyPacket* pReadyPkt = new ReadyPacket(1);
	m_pRakNetInstance->send(pReadyPkt);
#endif

	arrangeRequestChunkOrder();
	requestNextChunk();
}

void ClientSideNetworkHandler::onConnect(const RakNet::RakNetGUID& rakGuid) // server guid
{
	RakNet::RakNetGUID localGuid = ((RakNet::RakPeer*)m_pServerPeer)->GetMyGUID();
	printf_ignorable("onConnect, server guid: %s, local guid: %s", rakGuid.ToString(), localGuid.ToString());

	m_serverGUID = rakGuid;

	LoginPacket* pLoginPkt = new LoginPacket;
	pLoginPkt->m_str = RakNet::RakString(m_pMinecraft->m_pUser->field_0.c_str());
	pLoginPkt->m_clientNetworkVersion = NETWORK_PROTOCOL_VERSION;
	pLoginPkt->m_clientNetworkVersion2 = NETWORK_PROTOCOL_VERSION;
	
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
	puts_ignorable("SetTimePacket");

	if (m_pLevel)
		m_pLevel->setTime(pPacket->m_time);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, StartGamePacket* pStartGamePkt)
{
	puts_ignorable("StartGamePacket");

	m_pMinecraft->getLevelSource()->deleteLevel("_LastJoinedServer");

	m_pLevel = new Level(
		m_pMinecraft->getLevelSource()->selectLevel("_LastJoinedServer", true), 
		"temp",
		pStartGamePkt->m_seed,
		pStartGamePkt->m_levelVersion);

	m_pLevel->m_bIsClientSide = true;

	GameType gameType = pStartGamePkt->m_gameType != GAME_TYPES_MAX ? pStartGamePkt->m_gameType : m_pLevel->getDefaultGameType();
	LocalPlayer *pLocalPlayer = new LocalPlayer(m_pMinecraft, m_pLevel, m_pMinecraft->m_pUser, gameType, m_pLevel->m_pDimension->field_50);
	pLocalPlayer->m_guid = ((RakNet::RakPeer*)m_pServerPeer)->GetMyGUID();
	pLocalPlayer->m_EntityID = pStartGamePkt->m_entityId;
	
	pLocalPlayer->moveTo(
		pStartGamePkt->m_pos,
		pLocalPlayer->m_rot);

	if (gameType == GAME_TYPE_CREATIVE)
		pLocalPlayer->m_pInventory->prepareCreativeInventory();
	else
		pLocalPlayer->m_pInventory->prepareSurvivalInventory();

	m_pLevel->setTime(pStartGamePkt->m_time);

	m_serverProtocolVersion = pStartGamePkt->m_serverVersion;

	m_pMinecraft->setLevel(m_pLevel, "ClientSideNetworkHandler -> setLevel", pLocalPlayer);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, AddPlayerPacket* pAddPlayerPkt)
{
	puts_ignorable("AddPlayerPacket");

	if (!m_pLevel) return;

	Player* pPlayer = new Player(m_pLevel, m_pLevel->getDefaultGameType());
	pPlayer->m_EntityID = pAddPlayerPkt->m_id;
	m_pLevel->addEntity(pPlayer);

	pPlayer->moveTo(
		pAddPlayerPkt->m_pos,
		pPlayer->m_rot);

	pPlayer->m_name = pAddPlayerPkt->m_name;
	pPlayer->m_guid = pAddPlayerPkt->m_guid;

	if (pPlayer->getPlayerGameType() == GAME_TYPE_CREATIVE)
		pPlayer->m_pInventory->prepareCreativeInventory();
	else
		pPlayer->m_pInventory->prepareSurvivalInventory();

	m_pMinecraft->m_gui.addMessage(pPlayer->m_name + " joined the game");
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, RemoveEntityPacket* pRemoveEntityPkt)
{
	if (!m_pLevel) return;

	Entity* pEnt = m_pLevel->getEntity(pRemoveEntityPkt->m_id);

	if (pEnt)
		m_pLevel->removeEntity(pEnt);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, MovePlayerPacket* packet)
{
	if (!m_pLevel) return;

	Entity* pEntity = m_pLevel->getEntity(packet->m_id);
	if (!pEntity)
	{
		LOG_E("No player with id %d", packet->m_id);
		return;
	}
	
	pEntity->lerpTo(packet->m_pos, packet->m_rot, 3);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, PlaceBlockPacket* pPlaceBlockPkt)
{
	puts_ignorable("PlaceBlockPacket");

	Player* pPlayer = (Player*)m_pLevel->getEntity(pPlaceBlockPkt->m_entityId);
	if (!pPlayer)
	{
		LOG_E("No player with id %d", pPlaceBlockPkt->m_entityId);
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
		LOG_E("No player with id %d", pRemoveBlockPkt->m_entityId);
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
	if (!areAllChunksLoaded())
	{
		m_bufferedBlockUpdates.push_back(SBufferedBlockUpdate(pkt->m_pos, pkt->m_tileTypeId, pkt->m_data));
		return;
	}

	m_pLevel->setTileAndData(pkt->m_pos, pkt->m_tileTypeId, pkt->m_data);
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

	for (int k = 0; k < 256; k++)
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

	if (m_serverProtocolVersion < 2)
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
			cdp.read(&bs2);

			if (pChunk)
				handle(guid, &cdp);

			// Handle lighting immediately, to ensure it doesn't get out of control.
			while (m_pLevel->updateLights());
		}
	}

	// All chunks are loaded. Also flush all the updates we've buffered.
	m_chunksRequested = 256;
	flushAllBufferedUpdates();
}

bool ClientSideNetworkHandler::areAllChunksLoaded()
{
	return m_chunksRequested > 255;
}

void ClientSideNetworkHandler::arrangeRequestChunkOrder()
{
	clearChunksLoaded();
	// @TODO: Implement arrangeRequestChunkOrder()
}

void ClientSideNetworkHandler::clearChunksLoaded()
{
	// @TODO: Implement clearChunksLoaded()
}

void ClientSideNetworkHandler::requestNextChunk()
{
	if (areAllChunksLoaded())
		return;

	// @BUG: The return value of areAllChunksLoaded() is actually true even before the
	// 256th chunk is loaded.

	if (m_serverProtocolVersion < 2)
	{
		m_pRakNetInstance->send(new RequestChunkPacket(ChunkPos(m_chunksRequested % 16, m_chunksRequested / 16)));
		m_chunksRequested++;
	}
	else
	{
		m_pRakNetInstance->send(new RequestChunkPacket(ChunkPos(-9999, -9999)));
	}
}

void ClientSideNetworkHandler::flushAllBufferedUpdates()
{
	for (int i = 0; i < int(m_bufferedBlockUpdates.size()); i++)
	{
		SBufferedBlockUpdate& u = m_bufferedBlockUpdates[i];
		m_pLevel->setTileAndData(u.pos, u.tile, u.data);
	}
}
