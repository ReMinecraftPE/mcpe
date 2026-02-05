#include "ExplodePacket.hpp"
#include "network/NetEventCallback.hpp"

ExplodePacket::ExplodePacket(const Vec3& pos, float range)
	: m_pos(pos)
	, m_range(range)
{}

void ExplodePacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void ExplodePacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_EXPLODE);
	bs.Write(m_pos);
	bs.Write(m_range);
}

void ExplodePacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_pos);
	bs.Read(m_range);
}