/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ReadyPacket.hpp"
#include "network/NetEventCallback.hpp"

void ReadyPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void ReadyPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_READY);
	bs.Write(m_ready);
}

void ReadyPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_ready);
}
