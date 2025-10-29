/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "AddPlayerPacket.hpp"
#include "network/NetEventCallback.hpp"
#include "network/PacketUtil.hpp"
#include "world/entity/Player.hpp"

AddPlayerPacket::AddPlayerPacket(const Player *player)
{
	m_guid = player->m_guid;
	m_name = RakNet::RakString(player->m_name.c_str());
	m_id = player->m_EntityID;
	m_pos = player->m_pos;
	m_pos.y -= player->m_heightOffset;
	m_rot = player->m_rot;
	// apparently TILE_AIR isn't valid and causes a crash on 0.2.1
	// even though Mojang initializes this to TILE_AIR
	m_itemId = TILE_STONE;
	m_itemAuxValue = 0;
	ItemInstance* pItem = player->getSelectedItem();
	if (pItem)
	{
		m_itemId = pItem->getId();
		m_itemAuxValue = pItem->getAuxValue();
	}
}

void AddPlayerPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void AddPlayerPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_ADD_PLAYER);
	bs.Write(m_guid);
	bs.Write(m_name);
	bs.Write(m_id);
	bs.Write(m_pos.x);
	bs.Write(m_pos.y);
	bs.Write(m_pos.z);

#if NETWORK_PROTOCOL_VERSION >= 3
	bs.Write(PacketUtil::Rot_degreesToChar(m_rot.x));
	bs.Write(PacketUtil::Rot_degreesToChar(m_rot.y));
	bs.Write(m_itemId);
	bs.Write(m_itemAuxValue);
#endif
}

void AddPlayerPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_guid);
	bs.Read(m_name);
	bs.Read(m_id);
	bs.Read(m_pos.x);
	bs.Read(m_pos.y);
	bs.Read(m_pos.z);

#if NETWORK_PROTOCOL_VERSION >= 3
	char pitch, yaw;
	bs.Read(pitch);
	bs.Read(yaw);
	bs.Read(m_itemId);
	bs.Read(m_itemAuxValue);
	m_rot.x = PacketUtil::Rot_charToDegrees(pitch);
	m_rot.y = PacketUtil::Rot_charToDegrees(yaw);
#endif
}
