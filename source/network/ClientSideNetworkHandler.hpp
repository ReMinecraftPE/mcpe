/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "NetEventCallback.hpp"
#include "Minecraft.hpp"
#include "RakNetInstance.hpp"

struct SBufferedBlockUpdate
{
	int x, z;
	uint8_t y;
	uint8_t tile, data;

	SBufferedBlockUpdate(int x, int y, int z, TileID tile, int data) :
		x(x), y(uint8_t(y)), z(z), tile(uint8_t(tile)), data(uint8_t(data))
	{}
};

class ClientSideNetworkHandler : public NetEventCallback
{
public:
	ClientSideNetworkHandler(Minecraft*, RakNetInstance*);

	void levelGenerated(Level*) override;
	void onConnect(const RakNet::RakNetGUID&) override;
	void onDisconnect(const RakNet::RakNetGUID&) override;
	void onUnableToConnect() override;
	void handle(const RakNet::RakNetGUID&, MessagePacket*) override;
	void handle(const RakNet::RakNetGUID&, StartGamePacket*) override;
	void handle(const RakNet::RakNetGUID&, AddPlayerPacket*) override;
	void handle(const RakNet::RakNetGUID&, RemoveEntityPacket*) override;
	void handle(const RakNet::RakNetGUID&, MovePlayerPacket*) override;
	void handle(const RakNet::RakNetGUID&, PlaceBlockPacket*) override;
	void handle(const RakNet::RakNetGUID&, RemoveBlockPacket*) override;
	void handle(const RakNet::RakNetGUID&, UpdateBlockPacket*) override;
	void handle(const RakNet::RakNetGUID&, ChunkDataPacket*) override;
	void handle(const RakNet::RakNetGUID&, PlayerEquipmentPacket*) override;
	
	bool areAllChunksLoaded();
	void requestNextChunk();
	void flushAllBufferedUpdates(); // inlined

private:
	Minecraft* m_pMinecraft = NULL;
	Level* m_pLevel = NULL;
	RakNetInstance* m_pRakNetInstance = NULL;
	RakNet::RakPeerInterface* m_pServerPeer = NULL;
	int m_field_14;
	RakNet::RakNetGUID m_serverGUID;
	int m_field_24;
	std::vector<SBufferedBlockUpdate> m_bufferedBlockUpdates;
	int m_chunksRequested = 0;
};

