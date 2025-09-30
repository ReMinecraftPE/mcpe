/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "../Packet.hpp"

void StartGamePacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void StartGamePacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_START_GAME);
	bs->Write(m_seed);
	bs->Write(m_levelVersion);
#if NETWORK_PROTOCOL_VERSION >= 3
	bs->Write<int32_t>(m_gameType);
#endif
	bs->Write(m_entityId);
	bs->Write(m_pos.x);
	bs->Write(m_pos.y);
	bs->Write(m_pos.z);

	bs->Write(m_serverVersion);
	if (m_serverVersion >= 1)
	{
		bs->Write(m_time);
	}
}

void StartGamePacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_seed);
	bs->Read(m_levelVersion);
#if NETWORK_PROTOCOL_VERSION >= 3
	int32_t gameType;
	bs->Read(gameType);
	m_gameType = (GameType)gameType;
#endif
	bs->Read(m_entityId);
	bs->Read(m_pos.x);
	bs->Read(m_pos.y);
	bs->Read(m_pos.z);

	if (!bs->Read(m_serverVersion))
		return;

	if (m_serverVersion < 1)
		return;
	
	bs->Read(m_time);
}