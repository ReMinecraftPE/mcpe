/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ServerSideNetworkHandler.hpp"
#include "common/Utils.hpp"

// This lets you make the server shut up and not log events in the debug console.
#define VERBOSE_SERVER

#if defined(ORIGINAL_CODE) || defined(VERBOSE_SERVER)
#define puts_ignorable(str) LOG_I(str)
#define printf_ignorable(str, ...) LOG_I(str, __VA_ARGS__)
#else
#define puts_ignorable(str)
#define printf_ignorable(str, ...)
#endif

ServerSideNetworkHandler::ServerSideNetworkHandler(Minecraft* minecraft, RakNetInstance* rakNetInstance)
{
	m_pMinecraft = minecraft;
	m_pLevel = nullptr;
	m_pRakNetInstance = rakNetInstance;
	allowIncomingConnections(false);
	m_pRakNetPeer = m_pRakNetInstance->getPeer();
	m_bAllowIncoming = false;

	setupCommands();
}

ServerSideNetworkHandler::~ServerSideNetworkHandler()
{
	if (m_pLevel)
		m_pLevel->removeListener(this);

	for (OnlinePlayerMap::iterator it = m_onlinePlayers.begin(); it != m_onlinePlayers.end(); ++it)
		delete it->second;

	m_onlinePlayers.clear();
}

void ServerSideNetworkHandler::levelGenerated(Level* level)
{
	m_pLevel = level;

	if (m_pMinecraft->m_pLocalPlayer)
	{
		m_pMinecraft->m_pLocalPlayer->m_guid = m_pRakNetPeer->GetMyGUID();
	}

	level->addListener(this);

	allowIncomingConnections(m_pMinecraft->getOptions()->m_bServerVisibleDefault);

	m_onlinePlayers[m_pMinecraft->m_pLocalPlayer->m_guid] = new OnlinePlayer(m_pMinecraft->m_pLocalPlayer, m_pMinecraft->m_pLocalPlayer->m_guid);
}

void ServerSideNetworkHandler::onNewClient(const RakNet::RakNetGUID& guid)
{
	printf_ignorable("onNewClient, client guid: %s", guid.ToString());
}

void ServerSideNetworkHandler::onDisconnect(const RakNet::RakNetGUID& guid)
{
	puts_ignorable("onDisconnect");

	OnlinePlayer* pOnlinePlayer = getPlayerByGUID(guid);

	if (!pOnlinePlayer)
		return;

	Player* pPlayer = pOnlinePlayer->m_pPlayer;

	// erase it from the map
	m_onlinePlayers.erase(m_onlinePlayers.find(guid)); // it better be in our map

	// tell everyone that they left the game
	displayGameMessage(pPlayer->m_name + " disconnected from the game");
	m_pRakNetInstance->send(new RemoveEntityPacket(pPlayer->m_EntityID));

	// remove it from our world
	m_pLevel->removeEntity(pPlayer);

	// delete the online player's entry.
	delete pOnlinePlayer;
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, LoginPacket* packet)
{
	if (!m_bAllowIncoming)
		return;

	puts_ignorable("LoginPacket");

	// if they're already online, fail
	if (getPlayerByGUID(guid))
	{
		LOG_E("That player is already in the world!");
		return;
	}

	Player* pPlayer = new Player(m_pLevel, m_pLevel->getLevelData()->getGameType());
	pPlayer->m_guid = guid;
	pPlayer->m_name = std::string(packet->m_str.C_String());

	m_onlinePlayers[guid] = new OnlinePlayer(pPlayer, guid);

	StartGamePacket sgp;
	sgp.m_seed = m_pLevel->getSeed();
	sgp.m_levelVersion = m_pLevel->getLevelData()->m_version;
	sgp.m_entityId = pPlayer->m_EntityID;
#ifdef TEST_GAMEMODE_REPLICATION
	sgp.m_entityGameType = pPlayer->getPlayerGameType();
#endif
	sgp.m_pos.x = pPlayer->m_pos.x;
	sgp.m_pos.y = pPlayer->m_pos.y - pPlayer->field_84;
	sgp.m_pos.z = pPlayer->m_pos.z;
	sgp.m_serverVersion = 2;
	sgp.m_time = m_pLevel->getTime();
	
	RakNet::BitStream sgpbs;
	sgp.write(&sgpbs);
	m_pRakNetPeer->Send(&sgpbs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, guid, false);

	// send the connecting player info about all other players in the world
	for (int i = 0; i < int(m_pLevel->m_players.size()); i++)
	{
		Player* player = m_pLevel->m_players[i];
		AddPlayerPacket app(player->m_guid, RakNet::RakString(player->m_name.c_str()), player->m_EntityID, player->m_pos.x, player->m_pos.y - player->field_84, player->m_pos.z);
		RakNet::BitStream appbs;
		app.write(&appbs);
		m_pRakNetPeer->Send(&appbs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, guid, false);
	}

	m_pLevel->addEntity(pPlayer);

	if (m_pMinecraft->m_pGameMode->isCreativeType())
		pPlayer->m_pInventory->prepareCreativeInventory();
	else
		pPlayer->m_pInventory->prepareSurvivalInventory();

	m_pMinecraft->m_gui.addMessage(pPlayer->m_name + " joined the game");

	AddPlayerPacket app(guid, RakNet::RakString(pPlayer->m_name.c_str()), pPlayer->m_EntityID, pPlayer->m_pos.x, pPlayer->m_pos.y - pPlayer->field_84, pPlayer->m_pos.z);
	RakNet::BitStream appbs;
	app.write(&appbs);
	m_pRakNetPeer->Send(&appbs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, guid, true);
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, MessagePacket* packet)
{
	OnlinePlayer* pOP = getPlayerByGUID(guid);
	if (!pOP)
	{
		LOG_W("MessagePacket: That jerk %s doesn't actually exist", guid.ToString());
		return;
	}

	// don't let players send empty messages
	std::string msg(packet->m_str.C_String());
	if (msg.empty())
		return;

	if (msg[0] == '/')
	{
		LOG_I("CMD: %s: %s", pOP->m_pPlayer->m_name.c_str(), msg.c_str());

		std::stringstream ss(msg);
		ss.get(); // skip the /
		std::vector<std::string> parms;
		std::string currparm, cmdname;
		// get cmd name
		std::getline(ss, cmdname, ' ');
		// get cmd parms
		while (std::getline(ss, currparm, ' '))
			parms.push_back(currparm);

		CommandFunction func;
		CommandMap::iterator iter = m_commands.find(cmdname);
		if (iter == m_commands.end())
		{
			sendMessage(pOP, "Unknown command. Type /? for a list of commands.");
			return;
		}

		func = iter->second;

		(this->*func)(pOP, parms);

		return;
	}

	LOG_I("MSG: <%s> %s", pOP->m_pPlayer->m_name.c_str(), msg.c_str());

	// send everyone the message
	std::string gameMessage = "<" + pOP->m_pPlayer->m_name + "> " + msg;
	displayGameMessage(gameMessage);
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
	Mob* pMob = (Mob*)m_pLevel->getEntity(packet->m_playerID);
	if (!pMob)
		return;

	TileID tile = packet->m_tile;
	int face = packet->m_face;
	int x = packet->m_x;
	int y = packet->m_y;
	int z = packet->m_z;

	printf_ignorable("PlaceBlockPacket: %d", tile);

	if (!m_pLevel->mayPlace(tile, x, y, z, true))
		return;

	if (m_pLevel->setTile(x, y, z, tile))
	{
		Tile::tiles[tile]->setPlacedOnFace(m_pLevel, x, y, z, face);
		Tile::tiles[tile]->setPlacedBy(m_pLevel, x, y, z, pMob);

		const Tile::SoundType* pSound = Tile::tiles[tile]->m_pSound;
		m_pLevel->playSound(float(x) + 0.5f, float(y) + 0.5f, float(z) + 0.5f, "step." + pSound->m_name, 0.5f * (pSound->volume + 1.0f), pSound->pitch * 0.8f);
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
	//int data = m_pLevel->getData(x, y, z);
	bool setTileResult = m_pLevel->setTile(x, y, z, TILE_AIR);
	if (pTile && setTileResult)
	{
		const Tile::SoundType* pSound = pTile->m_pSound;
		m_pMinecraft->m_pSoundEngine->play("step." + pSound->m_name, float(x) + 0.5f, float(y) + 0.5f, float(z) + 0.5f, 0.5f * (pSound->volume + 1.0f), pSound->pitch * 0.8f);

		// redistribute the packet only if needed
		redistributePacket(packet, guid);
	}
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, PlayerEquipmentPacket* packet)
{
	Player* pPlayer = (Player*)m_pLevel->getEntity(packet->m_playerID);
	if (!pPlayer)
	{
		LOG_W("No player with id %d", packet->m_playerID);
		return;
	}

#ifndef ORIGINAL_CODE
	if (!Item::items[packet->m_itemID])
	{
		LOG_W("That item %d doesn't actually exist!", packet->m_itemID);
		return;
	}
#endif

	if (pPlayer->m_guid == m_pRakNetPeer->GetMyGUID())
	{
		LOG_W("Attempted to modify local player's inventory");
		return;
	}

	pPlayer->m_pInventory->selectItemById(packet->m_itemID, C_MAX_HOTBAR_ITEMS);

	redistributePacket(packet, guid);
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, RequestChunkPacket* packet)
{
	puts_ignorable("RequestChunkPacket");

	if (packet->m_x == -9999)
	{
		m_pRakNetInstance->send(guid, new LevelDataPacket(m_pLevel));
		return;
	}

	LevelChunk* pChunk = m_pLevel->getChunk(packet->m_x, packet->m_z);
	if (!pChunk)
	{
		LOG_E("No chunk at %d, %d", packet->m_x, packet->m_z);
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
		m_pRakNetInstance->announceServer(m_pMinecraft->getOptions()->m_playerName);
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

void ServerSideNetworkHandler::sendMessage(const RakNet::RakNetGUID& guid, const std::string& msg)
{
	if (m_pRakNetPeer->GetMyGUID() == guid)
	{
		m_pMinecraft->m_gui.addMessage(msg);
		return;
	}

	m_pRakNetInstance->send(guid, new MessagePacket(msg));
}

void ServerSideNetworkHandler::sendMessage(OnlinePlayer* player, const std::string& msg)
{
	sendMessage(player->m_guid, msg);
}

void ServerSideNetworkHandler::redistributePacket(Packet* packet, const RakNet::RakNetGUID& source)
{
	RakNet::BitStream bs;
	packet->write(&bs);

	m_pRakNetPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, source, true);
}

OnlinePlayer* ServerSideNetworkHandler::getPlayerByGUID(const RakNet::RakNetGUID& guid)
{
	OnlinePlayerMap::iterator iter = m_onlinePlayers.find(guid);
	if (iter == m_onlinePlayers.end())
		return nullptr;

	return iter->second;
}

///////////////// In-Game Commands /////////////////

void ServerSideNetworkHandler::setupCommands()
{
	m_commands["?"]     = &ServerSideNetworkHandler::commandHelp;
	m_commands["help"]  = &ServerSideNetworkHandler::commandHelp;
	m_commands["stats"] = &ServerSideNetworkHandler::commandStats;
	m_commands["time"]  = &ServerSideNetworkHandler::commandTime;
	m_commands["seed"]  = &ServerSideNetworkHandler::commandSeed;
	m_commands["tp"]    = &ServerSideNetworkHandler::commandTP;
}

void ServerSideNetworkHandler::commandHelp(OnlinePlayer* player, const std::vector<std::string>& parms)
{
	std::stringstream ss;
	ss << ">> Available commands:";

	for (CommandMap::iterator it = m_commands.begin(); it != m_commands.end(); ++it)
	{
		ss << " /" << it->first;
	}

	sendMessage(player, ss.str());
}

void ServerSideNetworkHandler::commandStats(OnlinePlayer* player, const std::vector<std::string>& parms)
{
	if (!m_pLevel)
		return;

	std::stringstream ss;
	ss << "Server uptime: " << getTimeS() << " seconds.\n";
	ss << "Host's name: " << m_pMinecraft->m_pUser->field_0 << "\n";

	int nPlayers = int(m_onlinePlayers.size());
	if (nPlayers == 1)
		ss << "There is 1 player online.";
	else
		ss << "There are " << nPlayers << " players online.";

	sendMessage(player, ss.str());
}

void ServerSideNetworkHandler::commandTime(OnlinePlayer* player, const std::vector<std::string>& parms)
{
	if (!m_pLevel)
		return;

	if (!parms.empty())
	{
		int t = 0;
		if (!sscanf(parms[0].c_str(), "%d", &t))
		{
			sendMessage(player, "Usage: /time [new time value]");
			return;
		}

		m_pLevel->setTime(t);
		sendMessage(player, "Time has been set to " + parms[0]);

		return;
	}

	std::stringstream ss;
	ss << "In-game time: ";
	ss << m_pLevel->getTime();
	ss << ". Day " << m_pLevel->getTime() / 24000;
	sendMessage(player, ss.str());
}

void ServerSideNetworkHandler::commandSeed(OnlinePlayer* player, const std::vector<std::string>& parms)
{
	if (!m_pLevel)
		return;

	std::stringstream ss;
	ss << "World generation seed: ";
	ss << m_pLevel->getSeed();
	sendMessage(player, ss.str());
}

void ServerSideNetworkHandler::commandTP(OnlinePlayer* player, const std::vector<std::string>& parms)
{
	if (!m_pLevel)
		return;
    
	if (parms.size() != 3)
	{
		sendMessage(player, "Usage: /tp <x> <y> <z>");
		return;
	}
    
	if (player->m_pPlayer != this->m_pMinecraft->m_pLocalPlayer)
	{
		sendMessage(player, "Sorry, only the host can use this command at the moment");
		return;
	}
    
	Vec3 pos = player->m_pPlayer->getPos(1.0f);
    
	float x = pos.x, y = pos.y, z = pos.z;
    
	std::stringstream ss;
	if (parms[0] != "~")
	{
		ss.str(parms[0]);
		ss >> x;
	}
	if (parms[1] != "~")
	{
		ss.str(parms[1]);
		ss >> y;
	}
	if (parms[2] != "~")
	{
		ss.str(parms[2]);
		ss >> z;
	}
    
	ss.str(std::string());
	ss << "Teleported to " << x << ", " << y << ", " << z;
    
	player->m_pPlayer->setPos(x, y, z);
    
	sendMessage(player, ss.str());
}
