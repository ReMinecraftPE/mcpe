/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SetTimePacket.hpp"
#include "network/NetEventCallback.hpp"

void SetTimePacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void SetTimePacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_SET_TIME);
	bs.Write(m_time);
}

void SetTimePacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_time);
}
