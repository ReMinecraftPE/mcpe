#pragma once

#include "../Packet.hpp"
#include "world/phys/Vec2.hpp"
#include "world/entity/SynchedEntityData.hpp"

class SetEntityDataPacket : public Packet
{
public:
	SetEntityDataPacket()
	{
		m_entityId = 0;
		m_bIsIncoming = false;
	}
	SetEntityDataPacket(int32_t id, SynchedEntityData& data);
	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
	const SynchedEntityData::ItemsArray& getUnpackedData() const { return m_packedItems; }
public:
	int32_t m_entityId;
	bool m_bIsIncoming;
private:
	SynchedEntityData::ItemsArray m_packedItems;
};