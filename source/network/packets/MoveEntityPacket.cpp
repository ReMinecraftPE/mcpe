#include "MoveEntityPacket.hpp"
#include "network/NetEventCallback.hpp"

void MoveEntityPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void MoveEntityPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_MOVE_ENTITY);
}

void MoveEntityPacket::read(RakNet::BitStream& bs)
{
}
