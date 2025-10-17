/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "MessagePacket.hpp"
#include "network/NetEventCallback.hpp"

void MessagePacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void MessagePacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_MESSAGE);
	bs.Write(m_str);
}

void MessagePacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_str);
}