/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ServerSideNetworkHandler.hpp"

// This lets you make the server shut up and not log events in the debug console.
#define VERBOSE_SERVER

#if defined(ORIGINAL_CODE) || defined(VERBOSE_SERVER)
#define puts_ignorable(str) puts(str)
#define printf_ignorable(str, ...) printf(str, __VA_ARGS__)
#else
#define puts_ignorable(str)
#define printf_ignorable(str, ...)
#endif

ServerSideNetworkHandler::ServerSideNetworkHandler(Minecraft* minecraft, RakNetInstance* rakNetInstance)
{
	m_pMinecraft = minecraft;
	m_pRakNetInstance = rakNetInstance;
	allowIncomingConnections(false);
	m_pRakNetPeer = m_pRakNetInstance->getPeer();	
}

ServerSideNetworkHandler::~ServerSideNetworkHandler()
{
	if (m_pLevel)
		m_pLevel->removeListener(this);
}

void ServerSideNetworkHandler::levelGenerated(Level* level)
{
	m_pLevel = level;

	if (m_pMinecraft->m_pLocalPlayer)
	{
		m_pMinecraft->m_pLocalPlayer->m_guid = m_pRakNetPeer->GetMyGUID();
	}

	level->addListener(this);

	allowIncomingConnections(m_pMinecraft->m_options.m_bServerVisibleDefault);
}

void ServerSideNetworkHandler::onNewClient(const RakNet::RakNetGUID& guid)
{
	printf_ignorable("onNewClient, client guid: %s\n", guid.ToString());
}

void ServerSideNetworkHandler::onDisconnect(const RakNet::RakNetGUID& guid)
{
	puts_ignorable("onDisconnect");

	for (Player* pPlayer : m_pLevel->m_players)
	{
		if (pPlayer->m_guid != guid)
			continue;

		displayGameMessage(pPlayer->m_name + " disconnected from the game");

		m_pRakNetInstance->send(new RemoveEntityPacket(pPlayer->m_EntityID));

		m_pLevel->removeEntity(pPlayer);
	}
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, LoginPacket* packet)
{
	if (!m_bAllowIncoming)
		return;

	puts_ignorable("LoginPacket");

	Player* pPlayer = new Player(m_pLevel);
	pPlayer->m_guid = guid;
	pPlayer->m_name = std::string(packet->m_str.C_String());

	StartGamePacket sgp;
	sgp.field_4 = m_pLevel->getSeed();
	sgp.field_8 = m_pLevel->getLevelData()->field_20;
	sgp.field_C = pPlayer->m_EntityID;
	sgp.field_10 = pPlayer->m_pos.x;
	sgp.field_14 = pPlayer->m_pos.y - pPlayer->field_84;
	sgp.field_18 = pPlayer->m_pos.z;
	
	RakNet::BitStream sgpbs;
	sgp.write(&sgpbs);
	m_pRakNetPeer->Send(&sgpbs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, guid, false);

	// send the connecting player info about all other players in the world
	for (Player* player : m_pLevel->m_players)
	{
		AddPlayerPacket app(player->m_guid, RakNet::RakString(player->m_name.c_str()), player->m_EntityID, player->m_pos.x, player->m_pos.y - player->field_84, player->m_pos.z);
		RakNet::BitStream appbs;
		app.write(&appbs);
		m_pRakNetPeer->Send(&appbs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, guid, false);
	}

	m_pLevel->addEntity(pPlayer);

	m_pMinecraft->m_gui.addMessage(pPlayer->m_name + " joined the game");

	AddPlayerPacket app(guid, RakNet::RakString(pPlayer->m_name.c_str()), pPlayer->m_EntityID, pPlayer->m_pos.x, pPlayer->m_pos.y - pPlayer->field_84, pPlayer->m_pos.z);
	RakNet::BitStream appbs;
	app.write(&appbs);
	m_pRakNetPeer->Send(&appbs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, guid, true);
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, MovePlayerPacket* packet)
{
	//not in the original
	puts_ignorable("MovePlayerPacket");

	Entity* pEntity = m_pLevel->getEntity(packet->m_id);
	if (pEntity)
		pEntity->lerpTo(packet->m_x, packet->m_y, packet->m_z, packet->m_yaw, packet->m_pitch, 3);

	redistributePacket(packet, guid);
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, PlaceBlockPacket* packet)
{
	puts_ignorable("PlaceBlockPacket");

	Mob* pMob = (Mob*)m_pLevel->getEntity(packet->m_playerID);
	if (!pMob)
		return;

	TileID tile = packet->m_tile;
	int face = packet->m_face;
	int x = packet->m_x;
	int y = packet->m_y;
	int z = packet->m_z;

	if (!m_pLevel->mayPlace(tile, x, y, z, true))
		return;

	if (m_pLevel->setTile(x, y, z, tile))
	{
		Tile::tiles[tile]->setPlacedOnFace(m_pLevel, x, y, z, face);
		Tile::tiles[tile]->setPlacedBy(m_pLevel, x, y, z, pMob);

		const Tile::SoundType* pSound = Tile::tiles[tile]->m_pSound;
		m_pLevel->playSound(float(x) + 0.5f, float(y) + 0.5f, float(z) + 0.5f, "step." + pSound->m_name, 0.5f * (pSound->field_18 + 1.0f), pSound->field_1C * 0.8f);
	}

	redistributePacket(packet, guid);
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, RemoveBlockPacket* packet)
{
	puts_ignorable("RemoveBlockPacket");

	Player* pPlayer = (Player*)m_pLevel->getEntity(packet->m_playerID);
	if (!pPlayer)
		return;

	int x = packet->m_x;
	int y = packet->m_y;
	int z = packet->m_z;

	Tile* pTile = Tile::tiles[m_pLevel->getTile(x, y, z)];
	int data = m_pLevel->getData(x, y, z);
	bool setTileResult = m_pLevel->setTile(x, y, z, TILE_AIR);
	if (pTile && setTileResult)
	{
		const Tile::SoundType* pSound = pTile->m_pSound;
		m_pMinecraft->m_pSoundEngine->play("step." + pSound->m_name, float(x) + 0.5f, float(y) + 0.5f, float(z) + 0.5f, 0.5f * (pSound->field_18 + 1.0f), pSound->field_1C * 0.8f);

		// redistribute the packet only if needed
		redistributePacket(packet, guid);
	}
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, PlayerEquipmentPacket* packet)
{
	Player* pPlayer = (Player*)m_pLevel->getEntity(packet->m_playerID);
	if (!pPlayer)
	{
		LogMsg("No player with id %d", packet->m_playerID);
		return;
	}

#ifndef ORIGINAL_CODE
	if (!Item::items[packet->m_itemID])
	{
		LogMsg("That item %d doesn't actually exist!", packet->m_itemID);
		return;
	}
#endif

	if (pPlayer->m_guid == m_pRakNetPeer->GetMyGUID())
	{
		puts("Attempted to modify local player's inventory");
		return;
	}

	pPlayer->m_pInventory->setSelectionSlotItemId(0, packet->m_itemID);
	pPlayer->m_pInventory->selectSlot(0);

	redistributePacket(packet, guid);
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, RequestChunkPacket* packet)
{
	puts_ignorable("RequestChunkPacket");

	LevelChunk* pChunk = m_pLevel->getChunk(packet->m_x, packet->m_z);
	if (!pChunk)
	{
		LogMsg("No chunk at %d, %d", packet->m_x, packet->m_z);
		return;
	}

	// @NOTE: this allows the client to request empty chunks. Is that okay?
	ChunkDataPacket cdp(pChunk->m_chunkX, pChunk->m_chunkZ, pChunk);

	RakNet::BitStream bs;
	cdp.write(&bs);

	m_pRakNetPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, guid, false);
}

void ServerSideNetworkHandler::tileBrightnessChanged(int x, int y, int z)
{
}

void ServerSideNetworkHandler::tileChanged(int x, int y, int z)
{
	UpdateBlockPacket ubp;

	int tile = m_pLevel->getTile(x, y, z);
	int data = m_pLevel->getData(x, y, z);

	ubp.m_x = x;
	ubp.m_y = y;
	ubp.m_z = z;
	ubp.m_tile = uint8_t(tile);
	ubp.m_data = uint8_t(data);

	RakNet::BitStream bs;
	ubp.write(&bs);

	m_pRakNetPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::AddressOrGUID(), true);
}

void ServerSideNetworkHandler::allowIncomingConnections(bool b)
{
	if (b)
	{
		m_pRakNetInstance->announceServer(m_pMinecraft->m_options.m_playerName);
	}
	else
	{
		m_pRakNetInstance->announceServer("");
	}

	m_bAllowIncoming = b;
}

void ServerSideNetworkHandler::displayGameMessage(const std::string& msg)
{
	m_pMinecraft->m_gui.addMessage(msg);
	m_pRakNetInstance->send(new MessagePacket(msg));
}

void ServerSideNetworkHandler::redistributePacket(Packet* packet, const RakNet::RakNetGUID& source)
{
	RakNet::BitStream bs;
	packet->write(&bs);

	m_pRakNetPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, source, true);
}
