/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "PlayerEquipmentPacket.hpp"
#include "network/NetEventCallback.hpp"

void PlayerEquipmentPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void PlayerEquipmentPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_PLAYER_EQUIPMENT);
	bs.Write(m_playerID);
	bs.Write(m_itemID);
}

void PlayerEquipmentPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_playerID);
	bs.Read(m_itemID);
}
