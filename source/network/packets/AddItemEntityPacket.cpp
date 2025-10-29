#include "AddItemEntityPacket.hpp"
#include "network/NetEventCallback.hpp"

AddItemEntityPacket::AddItemEntityPacket(ItemEntity& itemEntity)
{
	m_entityId = itemEntity.m_EntityID;
	m_pos = itemEntity.m_pos;

	ItemInstance& itemInstance = *itemEntity.m_pItemInstance;
	m_itemId = itemInstance.getId();
	m_auxValue = itemInstance.getAuxValue();
	m_itemCount = itemInstance.m_count;

	m_velX = (itemEntity.m_vel.x * 128.0f);
	m_velY = (itemEntity.m_vel.y * 128.0f);
	m_velZ = (itemEntity.m_vel.z * 128.0f);

	itemEntity.m_vel.x = m_velX * (1.f / 128.f);
	itemEntity.m_vel.y = m_velY * (1.f / 128.f);
	itemEntity.m_vel.z = m_velZ * (1.f / 128.f);
}

void AddItemEntityPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback& callback)
{
	callback.handle(guid, this);
}

void AddItemEntityPacket::write(RakNet::BitStream& bs)
{
	bs.Write((unsigned char)PACKET_ADD_ITEM_ENTITY);
	bs.Write(m_entityId);
	bs.Write(m_itemId);
	bs.Write(m_itemCount);
	bs.Write(m_auxValue);
	bs.Write(m_pos.x);
	bs.Write(m_pos.y);
	bs.Write(m_pos.z);
	bs.Write(m_velX);
	bs.Write(m_velY);
	bs.Write(m_velZ);
}

void AddItemEntityPacket::read(RakNet::BitStream& bs)
{
	bs.Read(m_entityId);
	bs.Read(m_itemId);
	bs.Read(m_itemCount);
	bs.Read(m_auxValue);
	bs.Read(m_pos.x);
	bs.Read(m_pos.y);
	bs.Read(m_pos.z);
	bs.Read(m_velX);
	bs.Read(m_velY);
	bs.Read(m_velZ);
}
