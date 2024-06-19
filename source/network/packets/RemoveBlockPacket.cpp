/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "../Packet.hpp"

void RemoveBlockPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void RemoveBlockPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_REMOVE_BLOCK);
	bs->Write(m_playerID);
	bs->Write(m_pos);
}

void RemoveBlockPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_playerID);
	bs->Read(m_pos);
}
