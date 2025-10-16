#pragma once

#include "../Packet.hpp"
#include "world/item/ItemInstance.hpp"
#include "world/level/TilePos.hpp"

class UseItemPacket : public Packet
{
public:
	UseItemPacket()
	{
		m_tileFace = 0;
		m_entityId = 0;
		m_itemId = TILE_AIR;
		m_itemAuxValue = 0;
	}
	UseItemPacket(const TilePos& tilePos, int32_t tileFace, int32_t entityId, const ItemInstance* pItem = nullptr);

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;
public:
	TilePos m_tilePos;
	int32_t m_tileFace;
	int32_t m_entityId;
	int16_t m_itemId;
	int8_t m_itemAuxValue;
	ItemInstance m_item;
};