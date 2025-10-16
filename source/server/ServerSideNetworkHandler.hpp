/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <map>
#include "client/app/Minecraft.hpp"
#include "network/NetEventCallback.hpp"
#include "network/RakNetInstance.hpp"
#include "world/level/LevelListener.hpp"

class Minecraft;
class ServerSideNetworkHandler;

struct OnlinePlayer
{
	Player* m_pPlayer; // The player avatar this online player controls
	RakNet::RakNetGUID m_guid;

	OnlinePlayer(Player* p, const RakNet::RakNetGUID& guid) : m_pPlayer(p), m_guid(guid) {}
};

typedef void(ServerSideNetworkHandler::* CommandFunction)(OnlinePlayer* player, const std::vector<std::string>& parms);
typedef std::map<std::string, CommandFunction> CommandMap;
typedef std::map<RakNet::RakNetGUID, OnlinePlayer*> OnlinePlayerMap;
typedef std::map<RakNet::RakNetGUID, Player*> PlayerMap;

// @TODO: Rename to ServerNetworkHandler?
class ServerSideNetworkHandler : public NetEventCallback, public LevelListener
{
private:
	bool _checkPermissions(OnlinePlayer* player);
	bool _validateNum(OnlinePlayer* player, int value, int min, int max);

public:

	// @TODO: We can do the following to finally split Network code from Client code
	// ServerSideNetworkHandler(GameCallbacks *gameCallbacks, Level* level, GameMode* gameMode, RakNetInstance* rakNetInstance, PacketSender* packetSender, Player* localPlayer);
	ServerSideNetworkHandler(Minecraft* minecraft, RakNetInstance* rakNetInstance);
	~ServerSideNetworkHandler();

	// Overridden from NetEventCallback
	void levelGenerated(Level*) override;
	void onNewClient(const RakNet::RakNetGUID&) override;
	void onDisconnect(const RakNet::RakNetGUID&) override;
	void handle(const RakNet::RakNetGUID&, LoginPacket*) override;
	void handle(const RakNet::RakNetGUID&, ReadyPacket*) override;
	void handle(const RakNet::RakNetGUID&, MessagePacket*) override;
	void handle(const RakNet::RakNetGUID&, MovePlayerPacket*) override;
	void handle(const RakNet::RakNetGUID&, PlaceBlockPacket*) override;
	void handle(const RakNet::RakNetGUID&, RemoveBlockPacket*) override;
	void handle(const RakNet::RakNetGUID&, PlayerEquipmentPacket*) override;
	void handle(const RakNet::RakNetGUID&, InteractPacket*) override;
	void handle(const RakNet::RakNetGUID&, UseItemPacket*) override;
	void handle(const RakNet::RakNetGUID&, RequestChunkPacket*) override;
	void handle(const RakNet::RakNetGUID&, AnimatePacket*) override;
	void handle(const RakNet::RakNetGUID&, RespawnPacket*) override;

	// Overridden from LevelListener
	void tileBrightnessChanged(const TilePos& pos) override;
	void tileChanged(const TilePos& pos) override;
	void timeChanged(uint32_t time) override;
	void entityAdded(Entity* entity) override;
	void entityRemoved(Entity* entity) override;
	void levelEvent(Player* pPlayer, LevelEvent::ID eventId, const TilePos& pos, LevelEvent::Data data) override;

	void allowIncomingConnections(bool b);
	Player* popPendingPlayer(const RakNet::RakNetGUID& guid);
	void displayGameMessage(const std::string&);
	void sendMessage(const RakNet::RakNetGUID& guid, const std::string&);
	void sendMessage(OnlinePlayer*, const std::string&);
	void redistributePacket(Packet* packet, const RakNet::RakNetGUID& source);

	// Custom
	OnlinePlayer* getOnlinePlayerByGUID(const RakNet::RakNetGUID& guid);
	Player* getPendingPlayerByGUID(const RakNet::RakNetGUID& guid);
	bool canReplicateEntity(const Entity* pEntity) const;
	void setupCommands();

	// Commands
	void commandHelp     (OnlinePlayer*, const std::vector<std::string>&);
	void commandStats    (OnlinePlayer*, const std::vector<std::string>&);
	void commandTime     (OnlinePlayer*, const std::vector<std::string>&);
	void commandSeed     (OnlinePlayer*, const std::vector<std::string>&);
	void commandTP       (OnlinePlayer*, const std::vector<std::string>&);
	void commandSummon   (OnlinePlayer*, const std::vector<std::string>&);
	void commandGamemode (OnlinePlayer*, const std::vector<std::string>&);
	void commandGive     (OnlinePlayer*, const std::vector<std::string>&);
	void commandClear    (OnlinePlayer*, const std::vector<std::string>&);

public:
	Minecraft* m_pMinecraft;
	Level* m_pLevel;
	RakNetInstance* m_pRakNetInstance;
	RakNet::RakPeerInterface* m_pRakNetPeer;
	PlayerMap m_pendingPlayers;
	bool m_bAllowIncoming;

	OnlinePlayerMap m_onlinePlayers;
	CommandMap m_commands;
};

