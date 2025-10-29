#pragma once

#include "../Packet.hpp"
#include "world/phys/Vec2.hpp"
#include "world/entity/SynchedEntityData.hpp"

class AddMobPacket : public Packet
{
public:
	AddMobPacket()
	{
		m_entityId = 0;
		m_entityTypeId = 0;
	}
	AddMobPacket(const Mob& mob);
	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
	const SynchedEntityData::ItemsArray& getUnpackedData() const { return m_unpack; }
public:
	int32_t m_entityId;
	int32_t m_entityTypeId;
	Vec3 m_pos;
	Vec2 m_rot;
private:
	SynchedEntityData m_entityData;
	SynchedEntityData::ItemsArray m_unpack;
};