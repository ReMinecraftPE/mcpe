/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "RakNetInstance.hpp"

#include "common/Logger.hpp"
#include "thirdparty/raknet/GetTime.h"

#include "MinecraftPackets.hpp"
#include "NetEventCallback.hpp"

//#define LOG_PACKETS

#ifdef LOG_PACKETS
#define LOG_PACKET(str, ...) LOG_I(str, __VA_ARGS__)
#else
#define LOG_PACKET(str, ...)
#endif

RakNetInstance::RakNetInstance()
{
	m_bIsHost = false;
	m_pRakPeerInterface = RakNet::RakPeerInterface::GetInstance();
	m_pRakPeerInterface->SetOccasionalPing(true);
}

RakNetInstance::~RakNetInstance()
{
	if (m_pRakPeerInterface)
	{
		m_pRakPeerInterface->Shutdown(100);
		RakNet::RakPeerInterface::DestroyInstance(m_pRakPeerInterface);
		m_pRakPeerInterface = nullptr;
	}
}

void RakNetInstance::announceServer(const std::string& name)
{
	if (m_bIsHost && m_pRakPeerInterface->IsActive())
	{
		RakNet::RakString rs;
		rs += "MCCPP;Demo;"; // @NOTE: still adding 'Demo' despite not actually being a demo.
		rs += name.c_str();

		RakNet::BitStream bs;
		rs.Serialize(&bs);

		m_pRakPeerInterface->SetOfflinePingResponse((const char*)bs.GetData(), bs.GetNumberOfBytesUsed());
	}
}

void RakNetInstance::clearServerList()
{
	m_servers.clear();
}

bool RakNetInstance::connect(const char* host, int port)
{
	RakNet::SocketDescriptor sd;

	disconnect();
	
	if (m_pRakPeerInterface->Startup(4, &sd, 1) != RakNet::RAKNET_STARTED)
		return false;
    
    LOG_I("Connecting to %s", host);

	// Was evaluated to a bool, CONNECTION_ATTEMPT_STARTED is 0
	return m_pRakPeerInterface->Connect(host, port, nullptr, 0) != RakNet::CONNECTION_ATTEMPT_STARTED;
}

void RakNetInstance::disconnect()
{
	if (m_pRakPeerInterface->IsActive())
		m_pRakPeerInterface->Shutdown(500);

	m_bIsHost = false;
	m_bPingingForHosts = false;
}

RakNet::RakPeerInterface* RakNetInstance::getPeer()
{
	return m_pRakPeerInterface;
}

std::vector<PingedCompatibleServer>* RakNetInstance::getServerList()
{
	return &m_servers;
}

bool RakNetInstance::host(const std::string& name, int port, int maxConnections)
{
	if (m_pRakPeerInterface->IsActive())
		m_pRakPeerInterface->Shutdown(500);

	RakNet::SocketDescriptor sd(port, nullptr);

	m_pRakPeerInterface->SetMaximumIncomingConnections(maxConnections);
	int result = m_pRakPeerInterface->Startup(maxConnections, &sd, 1);
	
	m_bIsHost = true;
	m_bPingingForHosts = false;

	return result == RakNet::RAKNET_STARTED;
}

bool RakNetInstance::isMyLocalGuid(const RakNet::RakNetGUID& guid)
{
	if (!m_pRakPeerInterface->IsActive())
		return false;

	return m_pRakPeerInterface->GetMyGUID() == guid;
}

void RakNetInstance::pingForHosts(int port)
{
	if (!m_pRakPeerInterface->IsActive())
	{
		RakNet::SocketDescriptor sd;
		m_pRakPeerInterface->Startup(4, &sd, 1);
	}

	m_hostPingPort = port;
	m_bPingingForHosts = true;
	m_startedPingingAt = RakNet::GetTimeMS();

	m_pRakPeerInterface->Ping("255.255.255.255", port, true, 0);
}

void RakNetInstance::runEvents(NetEventCallback& callback)
{
	while (true)
	{
		RakNet::Packet* pPacket = m_pRakPeerInterface->Receive();

		if (!pPacket)
			break;
			
		uint8_t packetType = *(pPacket->data);

		RakNet::BitStream* pBitStream = new RakNet::BitStream(pPacket->data + 1, pPacket->length - 1, 0);
        
        LOG_PACKET("Recieved packet from %s (id: %d bitStream: 0x%x length: %u)", pPacket->systemAddress.ToString(), packetType, pBitStream, pPacket->length);

		// @NOTE: why -1?
		if (packetType >= PACKET_LOGIN - 1)
		{
			Packet* pUserPacket = MinecraftPackets::createPacket(packetType);
			if (pUserPacket)
			{
				pUserPacket->read(*pBitStream);
				//LOG_PACKET("Packet: %d", packetType);
				pUserPacket->handle(pPacket->guid, callback);
				delete pUserPacket;
			}
			else
			{
				LOG_W("Received unrecognized packet type: %d", packetType);
			}
		}
		else if (packetType > ID_DETECT_LOST_CONNECTIONS)
		switch (packetType)
		{
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				// @BUG: Two players sending connection requests at the same time could cause one of them to fail to connect
				m_guid = pPacket->guid;
				callback.onConnect(pPacket->guid);
				break;
			}
			case ID_CONNECTION_ATTEMPT_FAILED:
			{
				callback.onUnableToConnect();
				break;
			}
			case ID_NEW_INCOMING_CONNECTION:
			{
				callback.onNewClient(pPacket->guid);
				break;
			}
			case ID_DISCONNECTION_NOTIFICATION:
			case ID_CONNECTION_LOST:
			{
				callback.onDisconnect(pPacket->guid);
				break;
			}
			case ID_UNCONNECTED_PONG:
			{
				unsigned int thing;
				RakNet::RakString serverInfo, mccppDemo = "MCCPP;Demo;";
				pBitStream->Read(thing);
				pBitStream->Read(serverInfo);

				// check if the server info starts with MCCPP;Demo;
				if (serverInfo.GetLength() < mccppDemo.GetLength())
					break;

				RakNet::RakString headerSubstr = serverInfo.SubStr(0, unsigned(mccppDemo.GetLength()));
				if (mccppDemo.StrCmp(headerSubstr) != 0)
					break;

				// update the info of a pinged compatible server, if possible.
				for (int i = 0; i < m_servers.size(); i++)
				{
					PingedCompatibleServer& server = m_servers.at(i);
					if (server.m_address == pPacket->systemAddress)
					{
						server.m_lastPinged = RakNet::GetTimeMS();
						server.m_name = serverInfo.SubStr(unsigned(mccppDemo.GetLength()), unsigned(serverInfo.GetLength() - mccppDemo.GetLength()));
						goto SKIP_ADDING_NEW_ENTRY;
					}
				}

				// Add a new entry.
				{
					PingedCompatibleServer pcs;
					pcs.m_address    = pPacket->systemAddress;
					pcs.m_lastPinged = RakNet::GetTimeMS();
					pcs.m_name       = serverInfo.SubStr(unsigned(mccppDemo.GetLength()), unsigned(serverInfo.GetLength() - mccppDemo.GetLength()));
					m_servers.push_back(pcs);
				}


			SKIP_ADDING_NEW_ENTRY:
				break;
			}
			default:
				LOG_W("Received unrecognized RakNet packet type: %d", packetType);
				break;
		}

		m_pRakPeerInterface->DeallocatePacket(pPacket);
		if (pBitStream)
			delete pBitStream;
	}

	if (m_bPingingForHosts)
	{
		int timeDiff = RakNet::GetTimeMS() - m_startedPingingAt;
		if (timeDiff > 1000)
		{
			for (std::vector<PingedCompatibleServer>::iterator it = m_servers.begin(); it != m_servers.end(); )
			{
				if (RakNet::GetTimeMS() - it->m_lastPinged <= 3000)
				{
					it++;
					continue;
				}

				it = m_servers.erase(it);
			}

			pingForHosts(m_hostPingPort);
		}
	}
}

// this broadcasts a packet to all other connected peers
void RakNetInstance::send(Packet* packet)
{
	RakNet::BitStream bs;
	packet->write(bs);

    uint32_t result;
	if (m_bIsHost)
	{
		result = m_pRakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE, 0, RakNet::UNASSIGNED_RAKNET_GUID, true);
	}
	else
	{
		// send it to the host instead
		result = m_pRakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE, 0, m_guid, false);
	}
    
    if (result != 0)
    {
#ifdef LOG_PACKETS
    uint8_t packetId;
    bs.Read(packetId);
    LOG_PACKET("Sent packet (id: %d guid: %s)", packetId, m_bIsHost ? "UNASSIGNED_SYSTEM_ADDRESS" : m_guid.ToString());
#endif
    }
    else
    {
        LOG_E("Failed to send packet!");
    }

	delete packet;
}

// this sends a specific peer a message
void RakNetInstance::send(const RakNet::RakNetGUID& guid, Packet* packet)
{
	RakNet::BitStream bs;
	packet->write(bs);

	m_pRakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE, 0, guid, false);

	delete packet;
	// return 1300; --- ida tells me this returns 1300. Huh
}

void RakNetInstance::stopPingForHosts()
{
	if (!m_bPingingForHosts)
		return;

	m_pRakPeerInterface->Shutdown(0);
	m_bPingingForHosts = false;
}


