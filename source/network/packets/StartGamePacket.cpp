/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "../Packet.hpp"

void StartGamePacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void StartGamePacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_START_GAME);
	bs->Write(field_4);
	bs->Write(field_8);
	bs->Write(field_C);
	bs->Write(field_10);
	bs->Write(field_14);
	bs->Write(field_18);

	bs->Write(m_version);
	if (m_version >= 1)
	{
		bs->Write(m_time);
	}
}

void StartGamePacket::read(RakNet::BitStream* bs)
{
	bs->Read(field_4);
	bs->Read(field_8);
	bs->Read(field_C);
	bs->Read(field_10);
	bs->Read(field_14);
	bs->Read(field_18);

	if (!bs->Read(m_version))
		return;

	if (m_version < 1)
		return;
	
	bs->Read(m_time);
}