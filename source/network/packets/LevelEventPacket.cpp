#include "LevelEventPacket.hpp"
#include "network/NetEventCallback.hpp"

LevelEventPacket::LevelEventPacket(int16_t eventId, const TilePos& pos, int32_t data)
{
	m_eventId = eventId;
	m_pos = pos;
	m_data = data;
}

void LevelEventPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void LevelEventPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_LEVEL_EVENT);
	bs.Write(m_eventId);
	bs.Write((int16_t)m_pos.x);
	bs.Write((int16_t)m_pos.y);
	bs.Write((int16_t)m_pos.z);
	bs.Write(m_data);
}

void LevelEventPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_eventId);
	int16_t x, y, z;
	bs.Read(x);
	bs.Read(y);
	bs.Read(z);
	m_pos = TilePos(x, y, z);
	bs.Read(m_data);
}
