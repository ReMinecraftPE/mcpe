/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "PlaceBlockPacket.hpp"
#include "network/NetEventCallback.hpp"

void PlaceBlockPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void PlaceBlockPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_PLACE_BLOCK);
	bs.Write(m_entityId);

	// The order of the TilePos matters here
	bs.Write(m_pos.x);
	bs.Write(m_pos.z);
	bs.Write<uint8_t>(m_pos.y);

	bs.Write(m_face);
	bs.Write(m_tileTypeId);
#if NETWORK_PROTOCOL_VERSION >= 3
	bs.Write(m_data);
#endif
}

void PlaceBlockPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_entityId);

	// The order of the TilePos matters here
	bs.Read(m_pos.x);
	bs.Read(m_pos.z);
	uint8_t y;
	bs.Read(y);
	m_pos.y = y;

	bs.Read(m_face);
	bs.Read(m_tileTypeId);
#if NETWORK_PROTOCOL_VERSION >= 3
	bs.Read(m_data);
#endif
}
