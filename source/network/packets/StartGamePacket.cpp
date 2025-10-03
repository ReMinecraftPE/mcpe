/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "StartGamePacket.hpp"
#include "network/NetEventCallback.hpp"

void StartGamePacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void StartGamePacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_START_GAME);
	bs.Write(m_seed);
	bs.Write(m_levelVersion);
#if NETWORK_PROTOCOL_VERSION >= 3
	bs.Write<int32_t>(m_gameType);
#endif
	bs.Write(m_entityId);
	// probably older, you figure it out
#if NETWORK_PROTOCOL_VERSION >= 29
	bs.Write(m_time);
#endif
	bs.Write(m_pos.x);
	bs.Write(m_pos.y);
	bs.Write(m_pos.z);
}

void StartGamePacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_seed);
	bs.Read(m_levelVersion);
#if NETWORK_PROTOCOL_VERSION >= 3
	int32_t gameType;
	bs.Read(gameType);
	m_gameType = (GameType)gameType;
#endif
	bs.Read(m_entityId);
#if NETWORK_PROTOCOL_VERSION >= 29
	bs.Read(m_time);
#endif
	bs.Read(m_pos.x);
	bs.Read(m_pos.y);
	bs.Read(m_pos.z);
}