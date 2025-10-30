/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ServerSideNetworkHandler.hpp"
#include "common/Logger.hpp"
#include "GameMods.hpp"
#include "network/MinecraftPackets.hpp"
#include "world/entity/MobFactory.hpp"
#include "ServerPlayer.hpp"

// How frequently SetTimePackets are sent, in seconds.
// b1.3 sends every second. 0.2.1 seems to send every 12.
#define NETWORK_TIME_SEND_FREQUENCY 12

// This lets you make the server shut up and not log events in the debug console.
//#define VERBOSE_SERVER

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

	Player* pPlayer = nullptr;

	OnlinePlayer* pOnlinePlayer = getOnlinePlayerByGUID(guid);
	if (pOnlinePlayer)
	{
		// Player was in-game
		pPlayer = pOnlinePlayer->m_pPlayer;

		m_onlinePlayers.erase(guid);

		// delete the online player's entry.
		delete pOnlinePlayer;
		// pPlayer is managed by Level

		// tell everyone that they left the game
		displayGameMessage(pPlayer->m_name + " disconnected from the game");

		m_pRakNetInstance->send(new RemoveEntityPacket(pPlayer->m_EntityID));

		pPlayer->m_bForceRemove = true;
		// remove it from our world
		m_pLevel->removeEntity(pPlayer);
	}
	else if (pPlayer = getPendingPlayerByGUID(guid))
	{
		// Player was still loading
		m_pendingPlayers.erase(guid);

		// remove "it" from our heap
		delete pPlayer;
	}
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, LoginPacket* packet)
{
	if (!m_pLevel || !m_bAllowIncoming)
		return;

	puts_ignorable("LoginPacket");

	// if they're already online, fail
	if (getOnlinePlayerByGUID(guid))
	{
		LOG_E("That player is already in the world!");
		return;
	}

	RakNet::BitStream bs;

#if NETWORK_PROTOCOL_VERSION >= 3
	LoginStatusPacket::LoginStatus loginStatus = LoginStatusPacket::STATUS_SUCCESS;
	if (packet->m_clientNetworkVersion < NETWORK_PROTOCOL_VERSION_MIN)
	{
		loginStatus = LoginStatusPacket::STATUS_CLIENT_OUTDATED;
	}
	else if (packet->m_clientNetworkVersionMin > NETWORK_PROTOCOL_VERSION)
	{
		loginStatus = LoginStatusPacket::STATUS_SERVER_OUTDATED;
	}

	if (loginStatus != LoginStatusPacket::STATUS_SUCCESS)
	{
		LoginStatusPacket lsp = LoginStatusPacket(loginStatus);

		lsp.write(bs);
		m_pRakNetPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, guid, false);

		return;
	}
#endif

	ServerPlayer* pPlayer = new ServerPlayer(m_pLevel, m_pLevel->getLevelData()->getGameType());
	pPlayer->m_guid = guid;
	pPlayer->m_name = std::string(packet->m_userName.C_String());

	m_pendingPlayers[guid] = pPlayer;

	StartGamePacket sgp;
	sgp.m_seed = m_pLevel->getSeed();
	sgp.m_levelVersion = m_pLevel->getLevelData()->getStorageVersion();
	sgp.m_gameType = pPlayer->getPlayerGameType();
	sgp.m_entityId = pPlayer->m_EntityID;
	sgp.m_time = m_pLevel->getTime();
	sgp.m_pos = pPlayer->m_pos;
	sgp.m_pos.y -= pPlayer->m_heightOffset;
	
	sgp.write(bs);
	m_pRakNetPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, guid, false);

#if NETWORK_PROTOCOL_VERSION <= 2
	// emulate a ReadyPacket being received
	handle(guid, (ReadyPacket*)nullptr);
#endif
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, ReadyPacket* packet)
{
	if (!m_pLevel)
		return;

	if (packet)
	{
		// nullptr is emulated
		puts_ignorable("ReadyPacket");
	}

	Player* pPlayer = popPendingPlayer(guid);
	if (!pPlayer)
		return;

	RakNet::BitStream bs;

#if NETWORK_PROTOCOL_VERSION >= 3
	{
		SetTimePacket packet(m_pLevel->getTime());
		packet.write(bs);
		m_pRakNetPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, guid, false);
	}
#endif

	// send the connecting player info about all other players in the world
	for (int i = 0; i < int(m_pLevel->m_players.size()); i++)
	{
		Player* player = m_pLevel->m_players[i];
		AddPlayerPacket app(player);
		bs.Reset();
		app.write(bs);
		m_pRakNetPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, guid, false);
	}

	m_pLevel->addEntity(pPlayer);

	if (m_pMinecraft->m_pGameMode->isCreativeType())
		pPlayer->m_pInventory->prepareCreativeInventory();
	else
		pPlayer->m_pInventory->prepareSurvivalInventory();

	m_pMinecraft->m_gui.addMessage(pPlayer->m_name + " joined the game");

#if NETWORK_PROTOCOL_VERSION >= 3
	// send the connecting player info about all entities in the world
	for (int i = 0; i < int(m_pLevel->m_entities.size()); i++)
	{
		Entity* entity = m_pLevel->m_entities[i];
		if (canReplicateEntity(entity))
		{
			AddMobPacket packet(*((Mob*)entity));
			bs.Reset();
			packet.write(bs);
			m_pRakNetPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, guid, false);
		}
	}
#endif

	AddPlayerPacket app(pPlayer);
	bs.Reset();
	app.write(bs);
	m_pRakNetPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, guid, true);
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, MessagePacket* packet)
{
	OnlinePlayer* pOP = getOnlinePlayerByGUID(guid);
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
	//puts_ignorable("MovePlayerPacket");

	Entity* pEntity = m_pLevel->getEntity(packet->m_id);
	if (!pEntity)
		return;

	pEntity->lerpTo(packet->m_pos, packet->m_rot);

	redistributePacket(packet, guid);
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, PlaceBlockPacket* packet)
{
	if (!m_pLevel)
		return;

	Mob* pMob = (Mob*)m_pLevel->getEntity(packet->m_entityId);
	if (!pMob || !pMob->isPlayer())
		return;

	pMob->swing();

	TileID tileId = packet->m_tileTypeId;
	Facing::Name face = (Facing::Name)packet->m_face;
	TilePos pos = packet->m_pos;
	TileData data = packet->m_data;

	printf_ignorable("PlaceBlockPacket: %d", tileId);

	if (!m_pLevel->mayPlace(tileId, pos, true))
		return;

	if (m_pLevel->setTileAndData(pos, tileId, data))
	{
		Tile* pTile = Tile::tiles[tileId];
		pTile->setPlacedOnFace(m_pLevel, pos, face);
		pTile->setPlacedBy(m_pLevel, pos, pMob);

		const Tile::SoundType* pSound = pTile->m_pSound;
		m_pLevel->playSound(pos + 0.5f, "step." + pSound->m_name, 0.5f * (pSound->volume + 1.0f), pSound->pitch * 0.8f);
	}

	redistributePacket(packet, guid);
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, RemoveBlockPacket* packet)
{
	puts_ignorable("RemoveBlockPacket");

	Entity* pEntity = m_pLevel->getEntity(packet->m_entityId);
	if (!pEntity || !pEntity->isPlayer())
		return;

	Player* pPlayer = (Player*)pEntity;

	pPlayer->swing();

	TilePos pos = packet->m_pos;
	Tile* pTile = Tile::tiles[m_pLevel->getTile(pos)];
	int auxValue = m_pLevel->getData(pos);

	m_pMinecraft->m_pParticleEngine->destroyEffect(pos);

	bool setTileResult = m_pLevel->setTile(pos, TILE_AIR);
	if (pTile && setTileResult)
	{
		const Tile::SoundType* pSound = pTile->m_pSound;
		m_pLevel->playSound(pos + 0.5f, "step." + pSound->m_name, 0.5f * (pSound->volume + 1.0f), pSound->pitch * 0.8f);

		if (pPlayer->isSurvival())
		{
#ifdef MOD_POCKET_SURVIVAL
			// 0.2.1
			ItemInstance tileItem(pTile, 1, auxValue);
			if (pTile == Tile::grass || !pPlayer->m_pInventory->hasUnlimitedResource(&tileItem))
			{
				pTile->spawnResources(m_pLevel, pos, auxValue);
			}
#else
			pTile->spawnResources(m_pLevel, pos, auxValue);
#endif
		}

		pTile->destroy(m_pLevel, pos, auxValue);

		// redistribute the packet only if needed
		redistributePacket(packet, guid);
	}
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, PlayerEquipmentPacket* packet)
{
	Player* pPlayer = (Player*)m_pLevel->getEntity(packet->m_playerID);
	if (!pPlayer)
	{
		LOG_W("PlayerEquipmentPacket: No player with id %d", packet->m_playerID);
		return;
	}

	if (pPlayer->m_guid == m_pRakNetPeer->GetMyGUID())
	{
		LOG_W("Attempted to modify local player's inventory");
		return;
	}

	pPlayer->m_pInventory->selectItemById(packet->m_itemID, C_MAX_HOTBAR_ITEMS);

	redistributePacket(packet, guid);
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, InteractPacket* packet)
{
	//puts_ignorable("InteractPacket");
	if (!m_pLevel) return;

	Entity* pSource = m_pLevel->getEntity(packet->m_sourceId);
	Entity* pTarget = m_pLevel->getEntity(packet->m_targetId);
	if (!pSource || !pTarget)
		return;

	if (!pSource->isPlayer())
		return;

	Player* pPlayer = (Player*)pSource;
	switch (packet->m_actionType)
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
		LOG_W("Received unkown action in InteractPacket: %d", packet->m_actionType);
		break;
	}

	redistributePacket(packet, guid);
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, UseItemPacket* packet)
{
	//puts_ignorable("UseItemPacket");
	if (!m_pLevel) return;

	Entity* pEntity = m_pLevel->getEntity(packet->m_entityId);
	if (!pEntity) return;
	Player* pPlayer = (Player*)pEntity;

	if (!pEntity->isPlayer())
		return;

	Tile* pTile = Tile::tiles[m_pLevel->getTile(packet->m_tilePos)];
	if (pTile)
	{
		if (pTile == Tile::invisible_bedrock)
			return;

		// Interface with tile instead of using item
		if (pTile->use(m_pLevel, packet->m_tilePos, pPlayer))
		{
			pPlayer->swing();
			return;
		}
	}

	if (packet->m_item.isNull())
		return;

	packet->m_item.useOn(pPlayer, m_pLevel, packet->m_tilePos, (Facing::Name)packet->m_tileFace);
	pPlayer->swing();
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, RequestChunkPacket* packet)
{
	//puts_ignorable("RequestChunkPacket");

	if (packet->m_chunkPos.x == -9999)
	{
		m_pRakNetInstance->send(guid, new LevelDataPacket(m_pLevel));
		return;
	}

	LevelChunk* pChunk = m_pLevel->getChunk(packet->m_chunkPos);
	if (!pChunk)
	{
		LOG_E("No chunk at %d, %d", packet->m_chunkPos.x, packet->m_chunkPos.z);
		return;
	}

	// @NOTE: this allows the client to request empty chunks. Is that okay?
	ChunkDataPacket cdp(pChunk->m_chunkPos, pChunk);

	RakNet::BitStream bs;
	cdp.write(bs);

	m_pRakNetPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, guid, false);
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, AnimatePacket* packet)
{
	//puts_ignorable("AnimatePacket");

	if (!m_pLevel)
		return;

	Entity* pEntity = m_pLevel->getEntity(packet->m_entityId);
	if (!pEntity)
		return;

	if (!pEntity->isPlayer())
		return;
	Player* pPlayer = (Player*)pEntity;

	switch (packet->m_actionId)
	{
		case AnimatePacket::SWING:
		{
			pPlayer->swing();
			break;
		}
		case AnimatePacket::HURT:
		{
			pPlayer->animateHurt();
			break;
		}
		default:
		{
			LOG_W("Received unkown action in AnimatePacket: %d, EntityType: %s", packet->m_actionId, pEntity->getDescriptor().getEntityType().getName().c_str());
			break;
		}
	}

	redistributePacket(packet, guid);
}

void ServerSideNetworkHandler::handle(const RakNet::RakNetGUID& guid, RespawnPacket* packet)
{
	puts_ignorable("RespawnPacket");

	if (!m_pLevel)
		return;

	NetEventCallback::handle(*m_pLevel, guid, packet);

	redistributePacket(packet, guid);
}

void ServerSideNetworkHandler::tileBrightnessChanged(const TilePos& pos)
{
}

void ServerSideNetworkHandler::tileChanged(const TilePos& pos)
{
	UpdateBlockPacket ubp;
	ubp.m_pos = pos;
	ubp.m_tileTypeId = m_pLevel->getTile(pos);
	ubp.m_data = m_pLevel->getData(pos);

	RakNet::BitStream bs;
	ubp.write(bs);

	m_pRakNetPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::AddressOrGUID(), true);
}

void ServerSideNetworkHandler::timeChanged(uint32_t time)
{
	if ((time % (20 * NETWORK_TIME_SEND_FREQUENCY)) == 0)
	{
		m_pRakNetInstance->send(new SetTimePacket(time));
	}
}

void ServerSideNetworkHandler::entityAdded(Entity* entity)
{
	if (entity->getDescriptor().isType(EntityType::ITEM))
	{
		m_pRakNetInstance->send(new AddItemEntityPacket(*(ItemEntity*)entity));
	}
	else
	{
		if (!canReplicateEntity(entity))
			return;

		AddMobPacket packet(*((Mob*)entity));
		redistributePacket(&packet, m_pRakNetInstance->m_guid);
	}
}

void ServerSideNetworkHandler::entityRemoved(Entity* entity)
{
	RemoveEntityPacket packet(entity->m_EntityID);
	redistributePacket(&packet, m_pRakNetInstance->m_guid);
}

void ServerSideNetworkHandler::levelEvent(Player* pPlayer, LevelEvent::ID eventId, const TilePos& pos, LevelEvent::Data data)
{
	LevelEventPacket pkt(eventId, pos, data);

	if (pPlayer)
	{
		redistributePacket(&pkt, pPlayer->m_guid);
	}
	else
	{
		redistributePacket(&pkt, m_pMinecraft->m_pLocalPlayer->m_guid);
	}
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

Player* ServerSideNetworkHandler::popPendingPlayer(const RakNet::RakNetGUID& guid)
{
	if (!m_pLevel)
		return nullptr;

	Player* pPlayer = getPendingPlayerByGUID(guid);
	if (pPlayer)
	{
		m_pendingPlayers.erase(guid);
		m_onlinePlayers[guid] = new OnlinePlayer(pPlayer, guid);
	}

	return pPlayer;
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
	packet->write(bs);

	m_pRakNetPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, source, true);
}

OnlinePlayer* ServerSideNetworkHandler::getOnlinePlayerByGUID(const RakNet::RakNetGUID& guid)
{
	OnlinePlayerMap::iterator iter = m_onlinePlayers.find(guid);
	if (iter == m_onlinePlayers.end())
		return nullptr;

	return iter->second;
}

Player* ServerSideNetworkHandler::getPendingPlayerByGUID(const RakNet::RakNetGUID& guid)
{
	PlayerMap::iterator iter = m_pendingPlayers.find(guid);
	if (iter == m_pendingPlayers.end())
		return nullptr;

	return iter->second;
}

bool ServerSideNetworkHandler::canReplicateEntity(const Entity* pEntity) const
{
	if (!pEntity || !pEntity->isMob() || pEntity->isPlayer())
		return false;

	// All clients on V3 will just crash if an unknown 
	// EntityType ID is replicated in an AddMobPacket.
	// V4 and above has the neccessary nullptr check to not need this.
#if NETWORK_PROTOCOL_VERSION <= 3
	EntityType::ID entityTypeId = pEntity->getDescriptor().getEntityType().getId();
	switch (entityTypeId)
	{
	case EntityType::SHEEP:
	case EntityType::ZOMBIE:
	case EntityType::PIG:
		return true;
	default:
		return false;
	}
#endif

	return true;
}

///////////////// In-Game Commands /////////////////

void ServerSideNetworkHandler::setupCommands()
{
	m_commands["?"]        = &ServerSideNetworkHandler::commandHelp;
	m_commands["help"]     = &ServerSideNetworkHandler::commandHelp;
	m_commands["stats"]    = &ServerSideNetworkHandler::commandStats;
	m_commands["time"]     = &ServerSideNetworkHandler::commandTime;
	m_commands["seed"]     = &ServerSideNetworkHandler::commandSeed;
	m_commands["tp"]       = &ServerSideNetworkHandler::commandTP;
	m_commands["summon"]   = &ServerSideNetworkHandler::commandSummon;
	m_commands["gamemode"] = &ServerSideNetworkHandler::commandGamemode;
	m_commands["give"]     = &ServerSideNetworkHandler::commandGive;
	m_commands["clear"]    = &ServerSideNetworkHandler::commandClear;
}

bool ServerSideNetworkHandler::_checkPermissions(OnlinePlayer* player)
{
	if (player->m_pPlayer != m_pMinecraft->m_pLocalPlayer)
	{
		sendMessage(player, "Sorry, only the host can use this command at the moment");
		return false;
	}

	return true;
}

bool ServerSideNetworkHandler::_validateNum(OnlinePlayer* player, int value, int min, int max)
{
	if (value < min)
	{
		sendMessage(player, Util::format("The number you have entered (%d) is too small, it must be at least %d", value, min));
		return false;
	}
	else if (value > max)
	{
		sendMessage(player, Util::format("The number you have entered (%d) is too big, it must be at most %d", value, max));
		return false;
	}

	return true;
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
			sendMessage(player, "Usage: /time <value>");
			return;
		}

		if (!_checkPermissions(player)) return;

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
    
	if (!_checkPermissions(player)) return;
    
	Vec3 pos = player->m_pPlayer->getPos(1.0f);
    
	std::stringstream ss;
	if (parms[0] != "~")
	{
		ss.str(parms[0]);
		ss.clear();
		ss >> pos.x;
	}
	if (parms[1] != "~")
	{
		ss.str(parms[1]);
		ss.clear();
		ss >> pos.y;
	}
	if (parms[2] != "~")
	{
		ss.str(parms[2]);
		ss.clear();
		ss >> pos.z;
	}
    
	ss.clear();
    
	player->m_pPlayer->setPos(pos);
	pos = player->m_pPlayer->getPos(1.0f);

	ss << "Teleported to " << pos.x << ", " << pos.y << ", " << pos.z;
    
	sendMessage(player, ss.str());
}

void ServerSideNetworkHandler::commandSummon(OnlinePlayer* player, const std::vector<std::string>& parms)
{
	if (!m_pLevel)
		return;

	const int parmsSize = parms.size();

	if (parmsSize != 1 && parmsSize != 4 && parmsSize != 5)
	{
		sendMessage(player, "Usage: /summon <entity> [x] [y] [z] [amount]");
		return;
	}

	if (!_checkPermissions(player)) return;

	std::string entityName;
	std::stringstream ss;
	ss.str(parms[0]);
	ss >> entityName;

	const EntityTypeDescriptor* descriptor = EntityTypeDescriptor::GetByEntityTypeName(entityName);
	if (descriptor != nullptr)
	{
		Vec3 pos = player->m_pPlayer->getPos(1.0f);
		pos.y -= player->m_pPlayer->m_heightOffset + player->m_pPlayer->m_ySlideOffset;

		if (parmsSize >= 4)
		{
			if (parms[1] != "~")
			{
				ss.str(parms[1]);
				ss.clear();
				ss >> pos.x;
			}
			if (parms[2] != "~")
			{
				ss.str(parms[2]);
				ss.clear();
				ss >> pos.y;
			}
			if (parms[3] != "~")
			{
				ss.str(parms[3]);
				ss.clear();
				ss >> pos.z;
			}
		}

		int amount = 1;
		if (parmsSize >= 5)
		{
			ss.str(parms[4]);
			ss.clear();
			ss >> amount;
		}

		ss.clear();
		if (descriptor->getEntityType() != EntityType::UNKNOWN)
		{
			bool success = false;

			for (int i = 0; i++ < amount;)
			{
				Mob* mob = MobFactory::CreateMob(descriptor->getEntityType().getId(), m_pLevel);
				if (mob == nullptr)
				{
					ss << "Unable to summon object";
					break;
				}
				mob->setPos(pos);
				m_pLevel->addEntity(mob);
				if (!success) success = true;
			}

			if (success)
			{
				ss << "Object successfully summoned";
			}
		}
		else
		{
			ss << "Unable to summon unknown entity type";
		}
	}
	else
	{
		ss.clear();
		ss << "Unable to summon invalid entity type";
	}

	sendMessage(player, ss.str());
}

void ServerSideNetworkHandler::commandGamemode(OnlinePlayer* player, const std::vector<std::string>& parms)
{
	if (!m_pLevel)
		return;
    
	if (parms.size() != 1)
	{
		sendMessage(player, "Usage: /gamemode <mode>");
		return;
	}
	/*if (parms.size() < 1 || parms.size() > 2)
	{
		sendMessage(player, "Usage: /gamemode <mode> [player]");
		return;
	}*/
    
	if (!_checkPermissions(player)) return;
    
	Vec3 pos = player->m_pPlayer->getPos(1.0f);
    
	GameType gameMode;
	std::stringstream ss;
	ss.str(parms[0]);
	ss >> (int&)gameMode;

	if (!_validateNum(player, gameMode, GAME_TYPES_MIN, GAME_TYPES_MAX))
		return;
    
	player->m_pPlayer->setPlayerGameType(gameMode);
    
	sendMessage(player, "Your game mode has been updated");
}

void ServerSideNetworkHandler::commandGive(OnlinePlayer * player, const std::vector<std::string>&parms)
{
	const std::string usage = "Usage: /give <item> [amount] [data]";

	if (!m_pLevel)
		return;
	if (parms.size() < 1 || parms.size() > 3)
	{
		sendMessage(player, usage);
		return;
	}

	if (!_checkPermissions(player)) return;

	int id = 0;
	int amount = 1;
	int auxValue = 0;
	if (sscanf(parms[0].c_str(), "%d", &id))
	{
		if (!_validateNum(player, id, 1, C_MAX_ITEMS-1))
			return;
	}
	else
	{
		sendMessage(player, usage);
		return;
	}
	if (parms.size() >= 2 && sscanf(parms[1].c_str(), "%d", &amount))
	{
		if (!_validateNum(player, amount, 1, 64))
			return;
	}
	if (parms.size() >= 3 && sscanf(parms[2].c_str(), "%d", &auxValue))
	{
		if (!_validateNum(player, auxValue, 0, 255))
			return;
	}
	Item* item = Item::items[id];
	if (!item || id >= 512 || id < 0)
	{
		sendMessage(player, "There is no such item with ID " + parms[0]);
		return;
	}

	Inventory* pInventory = player->m_pPlayer->m_pInventory;

	pInventory->addTestItem(item->m_itemID, amount, auxValue);

	sendMessage(player, Util::format("Given %s (ID %d) * %d to %s", item->getName().c_str(), item->m_itemID, amount, player->m_pPlayer->m_name.c_str()));
	return;
}

void ServerSideNetworkHandler::commandClear(OnlinePlayer* player, const std::vector<std::string>& parms)
{
	if (!m_pLevel)
		return;
		
	if (!_checkPermissions(player)) return;

	Inventory* pInventory = player->m_pPlayer->m_pInventory;

	pInventory->empty(); // calling "clear" will delete all of our slots

	sendMessage(player, "Your inventory has been cleared.");
	return;
}
