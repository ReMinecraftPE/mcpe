/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "RemoveBlockPacket.hpp"
#include "network/NetEventCallback.hpp"

void RemoveBlockPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void RemoveBlockPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_REMOVE_BLOCK);
	bs.Write(m_entityId);

	// The order of the TilePos matters here
	bs.Write(m_pos.x);
	bs.Write(m_pos.z);
	bs.Write<uint8_t>(m_pos.y);
}

void RemoveBlockPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_entityId);

	// The order of the TilePos matters here
	bs.Read(m_pos.x);
	bs.Read(m_pos.z);
	uint8_t y;
	bs.Read(y);
	m_pos.y = y;
}
