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
	bs->Write(PACKET_MOVE_PLAYER);
	bs->Write(m_id);
	bs->Write(m_x);
	bs->Write(m_y);
	bs->Write(m_z);
	bs->Write(m_pitch);
	bs->Write(m_yaw);
}

void MovePlayerPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_id);
	bs->Read(m_x);
	bs->Read(m_y);
	bs->Read(m_z);
	bs->Read(m_pitch);
	bs->Read(m_yaw);
}
