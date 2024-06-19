/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "../Packet.hpp"

void UpdateBlockPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void UpdateBlockPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_UPDATE_BLOCK);
	bs->Write(m_pos);
	bs->Write(m_tile);
	bs->Write(m_data);
}

void UpdateBlockPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_pos);
	bs->Read(m_tile);
	bs->Read(m_data);
}
