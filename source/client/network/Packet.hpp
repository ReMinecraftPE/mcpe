/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include "client/common/LongHack.hpp"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "NetEventCallback.hpp"

class NetEventCallback;
class LevelChunk;

enum ePacketType : uint8_t
{
	PACKET_LOGIN = ID_USER_PACKET_ENUM,
	PACKET_MESSAGE,
	PACKET_START_GAME,
	PACKET_ADD_PLAYER,
	PACKET_REMOVE_ENTITY,
	PACKET_MOVE_PLAYER,
	PACKET_PLACE_BLOCK,
	PACKET_REMOVE_BLOCK,
	PACKET_UPDATE_BLOCK,
	PACKET_REQUEST_CHUNK,
	PACKET_CHUNK_DATA,
	PACKET_PLAYER_EQUIPMENT,
};

class Packet
{
public:
	virtual ~Packet() {}
	virtual void write(RakNet::BitStream*) = 0;
	virtual void read(RakNet::BitStream*) = 0;
	virtual void handle(const RakNet::RakNetGUID&, NetEventCallback*) = 0;
};

class LoginPacket : public Packet
{
public:
	LoginPacket() {}
	LoginPacket(const std::string& uname) { m_str = RakNet::RakString(uname.c_str()); }

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*);
	void read(RakNet::BitStream*);
public:
	RakNet::RakString m_str;
};

class MessagePacket : public Packet
{
public:
	MessagePacket() {}
	MessagePacket(const std::string& msg)
	{
		m_str = msg.c_str();
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*);
	void read(RakNet::BitStream*);
public:
	RakNet::RakString m_str;
};

class StartGamePacket : public Packet
{
public:
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*);
	void read(RakNet::BitStream*);
public:
	TLong field_4;
	int field_8;
	int field_C;
	float field_10;
	float field_14;
	float field_18;
};

class AddPlayerPacket : public Packet
{
public:
	AddPlayerPacket() {}
	AddPlayerPacket(const RakNet::RakNetGUID& guid, RakNet::RakString name, int id, float x, float y, float z);
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*);
	void read(RakNet::BitStream*);
public:
	int field_4;
	RakNet::RakNetGUID m_guid;
	int field_14;
	RakNet::RakString m_name;
	int m_id;
	float m_x;
	float m_y;
	float m_z;
};

class RemoveEntityPacket : public Packet
{
public:
	RemoveEntityPacket() {}
	RemoveEntityPacket(int id) { m_id = id; }

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*);
	void read(RakNet::BitStream*);
public:
	int m_id;
};

class MovePlayerPacket : public Packet
{
public:
	MovePlayerPacket() {}
	MovePlayerPacket(int id, float x, float y, float z, float pitch, float yaw): m_id(id), m_x(x), m_y(y), m_z(z), m_pitch(pitch), m_yaw(yaw) {}
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*);
	void read(RakNet::BitStream*);
public:
	int m_id;
	float m_x;
	float m_y;
	float m_z;
	float m_pitch;
	float m_yaw;
};

class PlaceBlockPacket : public Packet
{
public:
	PlaceBlockPacket() {}
	PlaceBlockPacket(int playerID, int x, uint8_t y, int z, uint8_t tile, uint8_t face)
	{
		m_playerID = playerID;
		m_x = x;
		m_y = y;
		m_z = z;
		m_tile = tile;
		m_face = face;
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*);
	void read(RakNet::BitStream*);
public:
	int m_playerID;
	int m_x;
	int m_z;
	uint8_t m_y;
	uint8_t m_tile;
	uint8_t m_face;
};

class RemoveBlockPacket : public Packet
{
public:
	RemoveBlockPacket() {}
	RemoveBlockPacket(int id, int x, int y, int z) :m_playerID(id), m_x(x), m_z(z), m_y(uint8_t(y)) {}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*);
	void read(RakNet::BitStream*);
public:
	int m_playerID;
	int m_x;
	int m_z;
	uint8_t m_y;
};

class UpdateBlockPacket : public Packet
{
public:
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*);
	void read(RakNet::BitStream*);
public:
	int m_x;
	int m_z;
	uint8_t m_y;
	uint8_t m_tile;
	uint8_t m_data;
};

class RequestChunkPacket : public Packet
{
public:
	RequestChunkPacket() {}
	RequestChunkPacket(int x, int z) { m_x = x; m_z = z; }
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*);
	void read(RakNet::BitStream*);
public:
	int m_x;
	int m_z;
};

class ChunkDataPacket : public Packet
{
public:
	ChunkDataPacket() {}
	ChunkDataPacket(int x, int z, LevelChunk* c) :m_x(x), m_z(z), m_pChunk(c) {}
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*);
	void read(RakNet::BitStream*);
public:
	int m_x;
	int m_z;
	RakNet::BitStream m_data;
	LevelChunk* m_pChunk;
};

class PlayerEquipmentPacket : public Packet
{
public:
	PlayerEquipmentPacket() {}
	PlayerEquipmentPacket(int playerID, int itemID): m_playerID(playerID), m_itemID(itemID) {}
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*);
	void read(RakNet::BitStream*);
public:
	int m_playerID;
	uint8_t m_itemID;
};
