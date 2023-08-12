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
	bs->Write(PACKET_UPDATE_BLOCK);
	bs->Write(m_x);
	bs->Write(m_z);
	bs->Write(m_y);
	bs->Write(m_tile);
	bs->Write(m_data);
}

void UpdateBlockPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_x);
	bs->Read(m_z);
	bs->Read(m_y);
	bs->Read(m_tile);
	bs->Read(m_data);
}
