/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LoginPacket.hpp"
#include "network/NetEventCallback.hpp"

void LoginPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void LoginPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_LOGIN);
	bs.Write(m_userName);
	bs.Write(m_clientNetworkVersion);
	bs.Write(m_clientNetworkVersionMin);
}

void LoginPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_userName);

	if (!bs.Read(m_clientNetworkVersion))
		return;
	bs.Read(m_clientNetworkVersionMin);
}
