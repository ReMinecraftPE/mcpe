/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "RakNetTypes.h"
#include "MinecraftPackets.hpp"
class Level;

class NetEventCallback
{
public:
	virtual ~NetEventCallback() {}

protected:
	Player* _findPlayer(Level& level, Entity::ID entityId = -1, const RakNet::RakNetGUID* guid = nullptr);

public:
	virtual void levelGenerated(Level*) {}
	virtual void onConnect(const RakNet::RakNetGUID&) {}
	virtual void onUnableToConnect() {}
	virtual void onNewClient(const RakNet::RakNetGUID&) {}
	virtual void onDisconnect(const RakNet::RakNetGUID&) {}

	// TODO: macro this with a global PacketType list or something
	virtual void handle(const RakNet::RakNetGUID&, LoginPacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, LoginStatusPacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, ReadyPacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, MessagePacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, SetTimePacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, StartGamePacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, AddMobPacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, AddPlayerPacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, RemoveEntityPacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, AddItemEntityPacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, TakeItemEntityPacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, MoveEntityPacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, MovePlayerPacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, PlaceBlockPacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, RemoveBlockPacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, UpdateBlockPacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, LevelEventPacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, EntityEventPacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, RequestChunkPacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, ChunkDataPacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, PlayerEquipmentPacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, InteractPacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, UseItemPacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, SetEntityDataPacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, SetHealthPacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, AnimatePacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, RespawnPacket*) {}
	virtual void handle(const RakNet::RakNetGUID&, LevelDataPacket*) {}

	virtual void handle(Level&, const RakNet::RakNetGUID&, RespawnPacket*);
};

