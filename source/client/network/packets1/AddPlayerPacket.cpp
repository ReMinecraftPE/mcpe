/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "../Packet.hpp"

AddPlayerPacket::AddPlayerPacket(const RakNet::RakNetGUID& guid, RakNet::RakString name, int id, float x, float y, float z)
{
	m_guid = guid;
	m_name = name;
	m_id = id;
	m_x = x;
	m_y = y;
	m_z = z;
}

void AddPlayerPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void AddPlayerPacket::write(RakNet::BitStream* bs)
{
	bs->Write(PACKET_ADD_PLAYER);
	bs->Write(m_guid);
	bs->Write(m_name);
	bs->Write(m_id);
	bs->Write(m_x);
	bs->Write(m_y);
	bs->Write(m_z);
}

void AddPlayerPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_guid);
	bs->Read(m_name);
	bs->Read(m_id);
	bs->Read(m_x);
	bs->Read(m_y);
	bs->Read(m_z);
}