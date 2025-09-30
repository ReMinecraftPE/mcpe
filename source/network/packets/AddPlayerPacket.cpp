/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "../Packet.hpp"

AddPlayerPacket::AddPlayerPacket(const Player *player)
{
	m_guid = player->m_guid;
	m_name = RakNet::RakString(player->m_name.c_str());
	m_id = player->m_EntityID;
	m_pos = player->m_pos;
	m_pos.y -= player->m_heightOffset;
}

void AddPlayerPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void AddPlayerPacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_ADD_PLAYER);
	bs->Write(m_guid);
	bs->Write(m_name);
	bs->Write(m_id);
	bs->Write(m_pos.x);
	bs->Write(m_pos.y);
	bs->Write(m_pos.z);
}

void AddPlayerPacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_guid);
	bs->Read(m_name);
	bs->Read(m_id);
	bs->Read(m_pos.x);
	bs->Read(m_pos.y);
	bs->Read(m_pos.z);
}
