/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "../Packet.hpp"

void MovePlayerPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void MovePlayerPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_MOVE_PLAYER);
	bs->Write(m_id);
	bs->Write(m_pos);
#if NETWORK_PROTOCOL_VERSION <= 2
	bs->Write(m_rot.y);
	bs->Write(m_rot.x);
#else
	bs->Write(m_rot);
#endif

}

void MovePlayerPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_id);
	bs->Read(m_pos);
#if NETWORK_PROTOCOL_VERSION <= 2
	bs->Read(m_rot.y);
	bs->Read(m_rot.x);
#else
	bs->Read(m_rot);
#endif
}
