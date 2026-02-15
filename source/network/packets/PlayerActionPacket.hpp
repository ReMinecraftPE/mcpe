#pragma once

#include "../Packet.hpp"
#include "world/level/TilePos.hpp"

class PlayerActionPacket : public Packet
{
public:
	enum Type
	{
		START_DESTROY_BLOCK,
		CONTINUE_DESTROY_BLOCK,
		STOP_DESTROY_BLOCK,
		GET_UPDATED_BLOCK,
		DROP_ITEM,
		STOP_USING_ITEM, // was removed at a later date
		START_SLEEPING,
		STOP_SLEEPING,
		RESPAWN,
		START_JUMP,
		START_SPRINTING,
		STOP_SPRINTING,
		START_SNEAKING,
		STOP_SNEAKING,
		CHANGE_DIMENSION,
		CHANGE_DIMENSION_ACK,
		START_GLIDING,
		STOP_GLIDING,
		DENY_DESTROY_BLOCK,
		CRACK_BLOCK,
		CHANGE_SKIN,
		UPDATED_ENCHANTING_SEED,
		START_SWIMMING,
		STOP_SWIMMING,
		START_SPIN_ATTACK,
		STOP_SPIN_ATTACK,
		START_BUILDING_BLOCK
	};

	PlayerActionPacket()
	{
		m_tileFace = Facing::DOWN;
		m_action = START_DESTROY_BLOCK;
		m_entityId = 0;
	}

	PlayerActionPacket(int32_t entityId, Type action, const TilePos& tilePos = TilePos::ZERO, Facing::Name tileFace = Facing::DOWN);

	void handle(const RakNet::RakNetGUID&, NetEventCallback& callback) override;
	void write(RakNet::BitStream&) override;
	void read(RakNet::BitStream&) override;

public:
	TilePos m_tilePos;
	Facing::Name m_tileFace;
	Type m_action;
	int32_t m_entityId;
};