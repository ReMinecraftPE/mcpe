/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include <vector>
#include "PingedCompatibleServer.hpp"
#include "BitStream.h"
#include "RakPeerInterface.h"
#include "Packet.hpp"

#define C_DEFAULT_PORT (19132)
#define C_MAX_CONNECTIONS (4) // pitiful

class Packet;

class RakNetInstance
{
public:
	RakNetInstance();
	~RakNetInstance();

protected:
	bool _startup(RakNet::SocketDescriptor& socketDesc, int maxConnections = C_MAX_CONNECTIONS);
	bool _tryStartup();

public:
	void announceServer(const std::string& name);
	void clearServerList();
	bool connect(const char* host, int port);
	void disconnect();
	RakNet::RakPeerInterface* getPeer();
	std::vector<PingedCompatibleServer>* getServerList();
	bool host(const std::string& name, int port, int maxConnections);
	bool isMyLocalGuid(const RakNet::RakNetGUID& guid);
	void pingForHosts(int port);
	void runEvents(NetEventCallback&);
	void send(Packet* packet);
	void send(const RakNet::RakNetGUID& guid, Packet* packet);
	void send(Packet& packet);
	void send(const RakNet::RakNetGUID& guid, Packet& packet);
	void stopPingForHosts();

public:
	RakNet::RakPeerInterface* m_pRakPeerInterface;
	bool m_bIsHost;
	int field_C;
	RakNet::RakNetGUID m_guid;
	int field_1C;
	std::vector<PingedCompatibleServer> m_servers;
	bool m_bPingingForHosts;
	int m_hostPingPort;
	int m_startedPingingAt;
};

