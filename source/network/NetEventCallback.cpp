/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "NetEventCallback.hpp"

void NetEventCallback::levelGenerated(Level* level)
{
}

void NetEventCallback::onConnect(const RakNet::RakNetGUID& guid)
{
}

void NetEventCallback::onUnableToConnect()
{
}

void NetEventCallback::onNewClient(const RakNet::RakNetGUID& guid)
{
}

void NetEventCallback::onDisconnect(const RakNet::RakNetGUID& guid)
{
}

void NetEventCallback::handle(const RakNet::RakNetGUID& guid, LoginPacket* packet)
{
}

void NetEventCallback::handle(const RakNet::RakNetGUID& guid, MessagePacket* packet)
{
}

void NetEventCallback::handle(const RakNet::RakNetGUID& guid, StartGamePacket* packet)
{
}

void NetEventCallback::handle(const RakNet::RakNetGUID& guid, AddPlayerPacket* packet)
{
}

void NetEventCallback::handle(const RakNet::RakNetGUID& guid, RemoveEntityPacket* packet)
{
}

void NetEventCallback::handle(const RakNet::RakNetGUID& guid, MovePlayerPacket* packet)
{
}

void NetEventCallback::handle(const RakNet::RakNetGUID& guid, PlaceBlockPacket* packet)
{
}

void NetEventCallback::handle(const RakNet::RakNetGUID& guid, RemoveBlockPacket* packet)
{
}

void NetEventCallback::handle(const RakNet::RakNetGUID& guid, UpdateBlockPacket* packet)
{
}

void NetEventCallback::handle(const RakNet::RakNetGUID& guid, RequestChunkPacket* packet)
{
}

void NetEventCallback::handle(const RakNet::RakNetGUID& guid, ChunkDataPacket* packet)
{
	
}

void NetEventCallback::handle(const RakNet::RakNetGUID& guid, PlayerEquipmentPacket* packet)
{
}
