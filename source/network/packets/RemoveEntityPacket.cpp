/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "RemoveEntityPacket.hpp"
#include "network/NetEventCallback.hpp"

void RemoveEntityPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void RemoveEntityPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_REMOVE_ENTITY);
	bs.Write(m_id);
}

void RemoveEntityPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_id);
}
