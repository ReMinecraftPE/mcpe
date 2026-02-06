#include "TileEventPacket.hpp"
#include "network/NetEventCallback.hpp"

TileEventPacket::TileEventPacket(const TilePos& pos, int32_t b0, int32_t b1)
	: m_pos(pos)
	, m_b0(b0)
	, m_b1(b1)
{}

void TileEventPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void TileEventPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_TILE_EVENT);
	bs.Write(m_pos.x);
	bs.Write(m_pos.y);
	bs.Write(m_pos.z);
	bs.Write(m_b0);
	bs.Write(m_b1);
}

void TileEventPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_pos.x);
	bs.Read(m_pos.y);
	bs.Read(m_pos.z);
	bs.Read(m_b0);
	bs.Read(m_b1);
}
