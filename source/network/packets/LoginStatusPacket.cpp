/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "../Packet.hpp"

void LoginStatusPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void LoginStatusPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_LOGIN_STATUS);
	bs->Write(m_loginStatus);
}

void LoginStatusPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_loginStatus);
}
