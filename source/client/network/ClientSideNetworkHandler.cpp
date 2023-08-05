/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <RakPeer.h>
#include "ClientSideNetworkHandler.hpp"

// This lets you make the client shut up and not log events in the debug console.
#define VERBOSE_CLIENT

#if defined(ORIGINAL_CODE) || defined(VERBOSE_CLIENT)
#define puts_ignorable(str) puts(str)
#define printf_ignorable(str, ...) printf(str, __VA_ARGS__)
#else
#define puts_ignorable(str)
#define printf_ignorable(str, ...)
#endif

ClientSideNetworkHandler::ClientSideNetworkHandler(Minecraft* pMinecraft, RakNetInstance* pRakNetInstance)
{
	m_pMinecraft = pMinecraft;
	m_pRakNetInstance = pRakNetInstance;
	m_pServerPeer = m_pRakNetInstance->getPeer();
}

void ClientSideNetworkHandler::levelGenerated(Level* level)
{
	m_pLevel = level;
	requestNextChunk();
}

void ClientSideNetworkHandler::onConnect(const RakNet::RakNetGUID& rakGuid) // server guid
{
	RakNet::RakNetGUID localGuid = ((RakNet::RakPeer*)m_pServerPeer)->GetMyGUID();
	printf_ignorable("onConnect, server guid: %s, local guid: %s\n", rakGuid.ToString(), localGuid.ToString());

	m_serverGUID = rakGuid;

	LoginPacket* pLoginPkt = new LoginPacket;
	pLoginPkt->m_str = RakNet::RakString(m_pMinecraft->m_pUser->field_0.c_str());
	
	m_pRakNetInstance->send(pLoginPkt);
}

void ClientSideNetworkHandler::onUnableToConnect()
{
	puts_ignorable("onUnableToConnect");
}

void ClientSideNetworkHandler::onDisconnect(const RakNet::RakNetGUID& rakGuid)
{
	puts_ignorable("onDisconnect");

	if (m_pLevel)
		m_pLevel->field_11 = false;

	m_pMinecraft->m_gui.addMessage("Disconnected from server");
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, MessagePacket* pMsgPkt)
{
	puts_ignorable("MessagePacket");

	m_pMinecraft->m_gui.addMessage(pMsgPkt->m_str.C_String());
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, StartGamePacket* pStartGamePkt)
{
	puts_ignorable("StartGamePacket");

	m_pMinecraft->getLevelSource()->deleteLevel("_LastJoinedServer");

	m_pLevel = new Level(
		m_pMinecraft->getLevelSource()->selectLevel("_LastJoinedServer", true), 
		"temp",
		pStartGamePkt->field_4,
		pStartGamePkt->field_8);

	m_pLevel->field_11 = true;

	auto pLocalPlayer = new LocalPlayer(m_pMinecraft, m_pLevel, m_pMinecraft->m_pUser, m_pLevel->m_pDimension->field_50);
	pLocalPlayer->m_guid = ((RakNet::RakPeer*)m_pServerPeer)->GetMyGUID();
	pLocalPlayer->m_EntityID = pStartGamePkt->field_C;
	
	pLocalPlayer->moveTo(
		pStartGamePkt->field_10,
		pStartGamePkt->field_14,
		pStartGamePkt->field_18,
		pLocalPlayer->m_yaw,
		pLocalPlayer->m_pitch);

	pLocalPlayer->m_pInventory->prepareCreativeInventory();

	m_pLevel->setTime(pStartGamePkt->m_time);

	m_pMinecraft->setLevel(m_pLevel, "ClientSideNetworkHandler -> setLevel", pLocalPlayer);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, AddPlayerPacket* pAddPlayerPkt)
{
	puts_ignorable("AddPlayerPacket");

	if (!m_pLevel) return;

	Player* pPlayer = new Player(m_pLevel);
	pPlayer->m_EntityID = pAddPlayerPkt->m_id;
	m_pLevel->addEntity(pPlayer);

	pPlayer->moveTo(
		pAddPlayerPkt->m_x,
		pAddPlayerPkt->m_y,
		pAddPlayerPkt->m_z,
		pPlayer->m_yaw,
		pPlayer->m_pitch);

	pPlayer->m_name = pAddPlayerPkt->m_name;
	pPlayer->m_guid = pAddPlayerPkt->m_guid;

	pPlayer->m_pInventory->prepareCreativeInventory();

	m_pMinecraft->m_gui.addMessage(pPlayer->m_name + " joined the game");
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, RemoveEntityPacket* pRemoveEntityPkt)
{
	if (!m_pLevel) return;

	Entity* pEnt = m_pLevel->getEntity(pRemoveEntityPkt->m_id);

#if defined(ORIGINAL_CODE) || UINTPTR_MAX == UINT32_MAX
	// @NOTE: On x64 systems, this won't print the right things.
	printf_ignorable("RemoveEntityPacket %d %d", pEnt, m_pMinecraft->m_pLocalPlayer);
#else
	printf_ignorable("RemoveEntityPacket %p %p", pEnt, m_pMinecraft->m_pLocalPlayer);
#endif

	if (pEnt)
		m_pLevel->removeEntity(pEnt);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, MovePlayerPacket* packet)
{
	if (!m_pLevel) return;

	Entity* pEntity = m_pLevel->getEntity(packet->m_id);
	if (!pEntity)
	{
		LogMsg("No player with id %d", packet->m_id);
		return;
	}
	
	pEntity->lerpTo(packet->m_x, packet->m_y, packet->m_z, packet->m_yaw, packet->m_pitch, 3);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, PlaceBlockPacket* pPlaceBlockPkt)
{
	puts_ignorable("PlaceBlockPacket");

	Player* pPlayer = (Player*)m_pLevel->getEntity(pPlaceBlockPkt->m_playerID);
	if (!pPlayer)
	{
		LogMsg("No player with id %d", pPlaceBlockPkt->m_playerID);
		return;
	}

	pPlayer->swing();

	// @BUG: Not buffering the update.
	if (!areAllChunksLoaded())
		return;

	int x = pPlaceBlockPkt->m_x;
	int y = pPlaceBlockPkt->m_y;
	int z = pPlaceBlockPkt->m_z;
	int tile = pPlaceBlockPkt->m_tile;
	int face = pPlaceBlockPkt->m_face;

	if (!m_pLevel->mayPlace(tile, x, y, z, true))
		return;

	Tile* pTile = Tile::tiles[tile];
	if (!m_pLevel->setTile(x, y, z, tile))
		return;

	Tile::tiles[tile]->setPlacedOnFace(m_pLevel, x, y, z, face);
	Tile::tiles[tile]->setPlacedBy(m_pLevel, x, y, z, pPlayer);

	const Tile::SoundType* pSound = pTile->m_pSound;
	m_pLevel->playSound(float(x) + 0.5f, float(y) + 0.5f, float(z) + 0.5f, "step." + pSound->m_name, 0.5f * (1.0f + pSound->field_18), 0.8f * pSound->field_1C);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, RemoveBlockPacket* pRemoveBlockPkt)
{
	puts_ignorable("RemoveBlockPacket");

	Player* pPlayer = (Player*)m_pLevel->getEntity(pRemoveBlockPkt->m_playerID);
	if (!pPlayer)
	{
		LogMsg("No player with id %d", pRemoveBlockPkt->m_playerID);
		return;
	}

	pPlayer->swing();

	// @BUG: Not buffering the update.
	if (!areAllChunksLoaded())
		return;

	int x = pRemoveBlockPkt->m_x;
	int y = pRemoveBlockPkt->m_y;
	int z = pRemoveBlockPkt->m_z;

	Tile* pTile = Tile::tiles[m_pLevel->getTile(x, y, z)];
	int data = m_pLevel->getData(x, y, z);
	bool setTileResult = m_pLevel->setTile(x, y, z, TILE_AIR);

	if (pTile && setTileResult)
	{
		const Tile::SoundType* pSound = pTile->m_pSound;
		m_pLevel->playSound(float(x) + 0.5f, float(y) + 0.5f, float(z) + 0.5f, "step." + pSound->m_name, 0.5f * (1.0f + pSound->field_18), 0.8f * pSound->field_1C);

		pTile->destroy(m_pLevel, x, y, z, data);
	}
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, UpdateBlockPacket* pkt)
{
	if (!areAllChunksLoaded())
	{
		m_bufferedBlockUpdates.push_back(SBufferedBlockUpdate(pkt->m_x, pkt->m_y, pkt->m_z, pkt->m_tile, pkt->m_data));
		return;
	}

	m_pLevel->setTileAndData(pkt->m_x, pkt->m_y, pkt->m_z, pkt->m_tile, pkt->m_data);
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, ChunkDataPacket* pChunkDataPkt)
{
	if (!m_pLevel)
	{
		puts("Level @ handle ChunkDataPacket is 0");
		return;
	}

	LevelChunk* pChunk = m_pLevel->getChunkSource()->create(pChunkDataPkt->m_x, pChunkDataPkt->m_z);
	if (!pChunk || pChunk->isEmpty())
	{
		puts("Failed to find write-able chunk");
		// @BUG: Not trying again.
		return;
	}

	int x16 = 16 * pChunkDataPkt->m_x;
	int z16 = 16 * pChunkDataPkt->m_z;

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
					m_pLevel->setTileNoUpdate(x16 + (k & 0xF), yPos + i, z16 + (k >> 4), tiles[i]);
				}

				int idx = ((k & 0xF) << 11) | ((k >> 4) << 7) + yPos;
				memcpy(&pChunk->m_tileData[idx >> 1], datas, sizeof datas);
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
		m_pLevel->setTilesDirty(minX + x16, minY, minZ, maxX + x16, maxY, maxZ + z16);

	pChunk->m_bUnsaved = true;

	if (areAllChunksLoaded())
		flushAllBufferedUpdates();
	else
		requestNextChunk();
}

void ClientSideNetworkHandler::handle(const RakNet::RakNetGUID& rakGuid, PlayerEquipmentPacket* pPlayerEquipmentPkt)
{
	Player* pPlayer = (Player*)m_pLevel->getEntity(pPlayerEquipmentPkt->m_playerID);
	if (!pPlayer)
		return;

#ifndef ORIGINAL_CODE
	if (!Item::items[pPlayerEquipmentPkt->m_itemID])
	{
		LogMsg("That item %d doesn't actually exist!", pPlayerEquipmentPkt->m_itemID);
		return;
	}
#endif

	if (pPlayer->m_guid == m_pServerPeer->GetMyGUID())
	{
		puts("Attempted to modify local player's inventory");
		return;
	}

	pPlayer->m_pInventory->selectItemById(pPlayerEquipmentPkt->m_itemID);
	pPlayer->m_pInventory->selectSlot(0);
}

bool ClientSideNetworkHandler::areAllChunksLoaded()
{
	return m_chunksRequested > 255;
}

void ClientSideNetworkHandler::requestNextChunk()
{
	if (areAllChunksLoaded())
		return;

	// @BUG: The return value of areAllChunksLoaded() is actually true even before the
	// 256th chunk is loaded.

	m_pRakNetInstance->send(new RequestChunkPacket(m_chunksRequested % 16, m_chunksRequested / 16));
	m_chunksRequested++;
}

void ClientSideNetworkHandler::flushAllBufferedUpdates()
{
	for (const SBufferedBlockUpdate& u : m_bufferedBlockUpdates)
	{
		m_pLevel->setTileAndData(u.x, u.y, u.z, u.tile, u.data);
	}
}
