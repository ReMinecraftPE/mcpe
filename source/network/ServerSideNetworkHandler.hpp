/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <map>
#include "NetEventCallback.hpp"
#include "Minecraft.hpp"
#include "RakNetInstance.hpp"
#include "world/level/LevelListener.hpp"

class Minecraft;
class ServerSideNetworkHandler;

struct OnlinePlayer
{
	Player* m_pPlayer; // The player avatar this online player controls
	RakNet::RakNetGUID m_guid;

	OnlinePlayer(Player* p, const RakNet::RakNetGUID& guid) : m_pPlayer(p), m_guid(guid) {}
};

struct RakNetGUIDHasher
{
	size_t operator()(const RakNet::RakNetGUID& guid) const
	{
		return size_t(guid.g);
	}
	bool operator()(const RakNet::RakNetGUID& guid1, const RakNet::RakNetGUID& guid2) const
	{
		return guid1 == guid2;
	}
};

typedef void(ServerSideNetworkHandler::* CommandFunction)(OnlinePlayer* player, const std::vector<std::string>& parms);
typedef std::map<std::string, CommandFunction> CommandMap;
typedef std::map<RakNet::RakNetGUID, OnlinePlayer*, RakNetGUIDHasher> OnlinePlayerMap;

class ServerSideNetworkHandler : public NetEventCallback, public LevelListener
{
public:
	ServerSideNetworkHandler(Minecraft*, RakNetInstance*);
	~ServerSideNetworkHandler();

	// Overridden from NetEventCallback
	void levelGenerated(Level*) override;
	void onNewClient(const RakNet::RakNetGUID&) override;
	void onDisconnect(const RakNet::RakNetGUID&) override;
	void handle(const RakNet::RakNetGUID&, LoginPacket*) override;
	void handle(const RakNet::RakNetGUID&, MessagePacket*) override;
	void handle(const RakNet::RakNetGUID&, MovePlayerPacket*) override;
	void handle(const RakNet::RakNetGUID&, PlaceBlockPacket*) override;
	void handle(const RakNet::RakNetGUID&, RemoveBlockPacket*) override;
	void handle(const RakNet::RakNetGUID&, PlayerEquipmentPacket*) override;
	void handle(const RakNet::RakNetGUID&, RequestChunkPacket*) override;

	// Overridden from LevelListener
	void tileBrightnessChanged(int x, int y, int z) override;
	void tileChanged(int x, int y, int z) override;

	void allowIncomingConnections(bool b);
	void displayGameMessage(const std::string&);
	void sendMessage(const RakNet::RakNetGUID& guid, const std::string&);
	void sendMessage(OnlinePlayer*, const std::string&);
	void redistributePacket(Packet* packet, const RakNet::RakNetGUID& source);

	// Custom
	OnlinePlayer* getPlayerByGUID(const RakNet::RakNetGUID& guid);
	void setupCommands();

	// Commands
	void commandHelp (OnlinePlayer*, const std::vector<std::string>&);
	void commandStats(OnlinePlayer*, const std::vector<std::string>&);
	void commandTime (OnlinePlayer*, const std::vector<std::string>&);
	void commandSeed (OnlinePlayer*, const std::vector<std::string>&);

public:
	Minecraft* m_pMinecraft;
	Level* m_pLevel;
	RakNetInstance* m_pRakNetInstance;
	RakNet::RakPeerInterface* m_pRakNetPeer;
	bool m_bAllowIncoming;

	OnlinePlayerMap m_onlinePlayers;
	CommandMap m_commands;
};

