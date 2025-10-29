/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "network/NetEventCallback.hpp"
#include "client/app/Minecraft.hpp"
#include "client/multiplayer/MultiPlayerLevel.hpp"
#include "network/RakNetInstance.hpp"

struct BlockUpdate
{
	TilePos pos;
	uint8_t tile, data;

	BlockUpdate(const TilePos& pos, TileID tile, TileData data) :
		pos(pos), tile(uint8_t(tile)), data(uint8_t(data))
	{}
};

// @TODO: Rename to ClientNetworkHandler?
class ClientSideNetworkHandler : public NetEventCallback
{
public:
	ClientSideNetworkHandler(Minecraft*, RakNetInstance*);

	void levelGenerated(Level*) override;
	void onConnect(const RakNet::RakNetGUID&) override;
	void onDisconnect(const RakNet::RakNetGUID&) override;
	void onUnableToConnect() override;
	void handle(const RakNet::RakNetGUID&, LoginStatusPacket*) override;
	void handle(const RakNet::RakNetGUID&, MessagePacket*) override;
	void handle(const RakNet::RakNetGUID&, SetTimePacket*) override;
	void handle(const RakNet::RakNetGUID&, StartGamePacket*) override;
	void handle(const RakNet::RakNetGUID&, AddPlayerPacket*) override;
	void handle(const RakNet::RakNetGUID&, AddMobPacket*) override;
	void handle(const RakNet::RakNetGUID&, RemoveEntityPacket*) override;
	void handle(const RakNet::RakNetGUID&, AddItemEntityPacket*) override;
	void handle(const RakNet::RakNetGUID&, TakeItemEntityPacket*) override;
	void handle(const RakNet::RakNetGUID&, MoveEntityPacket*) override;
	void handle(const RakNet::RakNetGUID&, MovePlayerPacket*) override;
	void handle(const RakNet::RakNetGUID&, PlaceBlockPacket*) override;
	void handle(const RakNet::RakNetGUID&, RemoveBlockPacket*) override;
	void handle(const RakNet::RakNetGUID&, UpdateBlockPacket*) override;
	void handle(const RakNet::RakNetGUID&, LevelEventPacket*) override;
	void handle(const RakNet::RakNetGUID&, EntityEventPacket*) override;
	void handle(const RakNet::RakNetGUID&, ChunkDataPacket*) override;
	void handle(const RakNet::RakNetGUID&, PlayerEquipmentPacket*) override;
	void handle(const RakNet::RakNetGUID&, InteractPacket*) override;
	void handle(const RakNet::RakNetGUID&, SetEntityDataPacket*) override;
	void handle(const RakNet::RakNetGUID&, SetHealthPacket*) override;
	void handle(const RakNet::RakNetGUID&, AnimatePacket*) override;
	void handle(const RakNet::RakNetGUID&, RespawnPacket*) override;
	void handle(const RakNet::RakNetGUID&, LevelDataPacket*) override;
	
	bool areAllChunksLoaded();
	bool isChunkLoaded(const ChunkPos& cp);
	void arrangeRequestChunkOrder();
	void clearChunksLoaded();
	void requestNextChunk();
	void flushAllBufferedUpdates(); // inlined
	void handleBlockUpdate(const BlockUpdate& u);

private:
	Minecraft* m_pMinecraft;
	MultiPlayerLevel* m_pLevel;
	RakNetInstance* m_pRakNetInstance;
	RakNet::RakPeerInterface* m_pServerPeer;
	int m_field_14;
	RakNet::RakNetGUID m_serverGUID;
	int m_field_24;
	std::vector<BlockUpdate> m_bufferedBlockUpdates;
	int m_chunksRequested;
	int m_chunkCount;
	ChunkPos m_orderedChunks[C_MAX_CHUNKS];
	bool m_chunkStates[C_MAX_CHUNKS_X][C_MAX_CHUNKS_Z];
	int m_serverProtocolVersion;
	bool m_bUseLevelDataPkt;
};

