/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include "world/phys/Vec3.hpp"
#include "world/gamemode/GameType.hpp"
#include "world/entity/Player.hpp"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "NetEventCallback.hpp"

#define NETWORK_PROTOCOL_VERSION_MIN 1 // ?
//#define NETWORK_PROTOCOL_VERSION 2	   // 0.1.1
#define NETWORK_PROTOCOL_VERSION 3	   // 0.2.1

class NetEventCallback;
class Level;
class LevelChunk;

// RakNet requires this to be cast to an "unsigned char" before being written to the BitStream
enum ePacketType
#ifndef USE_OLD_CPP
: uint8_t // this is compiled as a 32-bit integer in C++03 and earlier, and we obviously CANNOT afford a 24-bit inconsitency.
// TODO: WritePacketType function that casts this down to a uint8_t / an unsigned 8-bit integer?
#endif
{
#if NETWORK_PROTOCOL_VERSION <= 2
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

	PACKET_LEVEL_DATA = 200,

	// Used in future protocol versions
	PACKET_LOGIN_STATUS,
	PACKET_READY,
	PACKET_SET_TIME,
	PACKET_ADD_MOB,
	PACKET_REMOVE_PLAYER,
	PACKET_ADD_ITEM_ENTITY,
	PACKET_TAKE_ITEM_ENTITY,
	PACKET_MOVE_ENTITY,
	PACKET_MOVE_ENTITY_POS_ROT,
	PACKET_EXPLODE,
	PACKET_LEVEL_EVENT,
	PACKET_ENTITY_EVENT,
	PACKET_INTERACT,
	PACKET_USE_ITEM,
	PACKET_SET_ENTITY_DATA,
	PACKET_SET_HEALTH,
	PACKET_ANIMATE,
	PACKET_RESPAWN,
#else
	PACKET_LOGIN = ID_USER_PACKET_ENUM,
	PACKET_LOGIN_STATUS,
	PACKET_READY,
	PACKET_MESSAGE,
	PACKET_SET_TIME,
	PACKET_START_GAME,
	PACKET_ADD_MOB,
	PACKET_ADD_PLAYER,
	PACKET_REMOVE_PLAYER,
	PACKET_REMOVE_ENTITY = 144,
	PACKET_ADD_ITEM_ENTITY,
	PACKET_TAKE_ITEM_ENTITY,
	PACKET_MOVE_ENTITY,
	PACKET_MOVE_ENTITY_POS_ROT = 150,
	PACKET_MOVE_PLAYER,
	PACKET_PLACE_BLOCK,
	PACKET_REMOVE_BLOCK,
	PACKET_UPDATE_BLOCK,
	PACKET_EXPLODE,
	PACKET_LEVEL_EVENT,
	PACKET_ENTITY_EVENT,
	PACKET_REQUEST_CHUNK,
	PACKET_CHUNK_DATA,
	PACKET_PLAYER_EQUIPMENT,
	PACKET_INTERACT,
	PACKET_USE_ITEM,
	PACKET_SET_ENTITY_DATA,
	PACKET_SET_HEALTH,
	PACKET_ANIMATE,
	PACKET_RESPAWN,

	PACKET_LEVEL_DATA = 200,
#endif
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
	LoginPacket(const std::string& uname)
	{
		m_str = RakNet::RakString(uname.c_str());
		m_clientNetworkVersion = 2;
		m_clientNetworkVersion2 = 2;
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	RakNet::RakString m_str;
	int m_clientNetworkVersion;
	int m_clientNetworkVersion2;
};

class LoginStatusPacket : public Packet
{
public:
	enum LoginStatus
	{
		STATUS_SUCCESS,
		STATUS_CLIENT_OUTDATED,
		STATUS_SERVER_OUTDATED
	};

public:
	LoginStatusPacket(LoginStatus loginStatus = STATUS_SUCCESS)
	{
		m_loginStatus = loginStatus;
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	LoginStatus m_loginStatus;
};

class ReadyPacket : public Packet
{
public:
	ReadyPacket(int ready = 0)
	{
		m_ready = ready;
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	uint8_t m_ready;
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
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	RakNet::RakString m_str;
};

class SetTimePacket : public Packet
{
public:
	SetTimePacket(int32_t time = 0)
	{
		m_time = time;
	}

	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int32_t m_time;
};

class StartGamePacket : public Packet
{
public:
	StartGamePacket()
	{
		m_gameType = GAME_TYPES_MAX;
		m_serverVersion = 0;
		m_time = 0;
	}
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int32_t m_seed;
	int m_levelVersion;
	GameType m_gameType;
	int m_entityId;
	Vec3 m_pos;
	int m_serverVersion;
	int m_time;
};

class AddPlayerPacket : public Packet
{
public:
	AddPlayerPacket() {}
	AddPlayerPacket(const Player *player);
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
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
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int m_id;
};

class MovePlayerPacket : public Packet
{
public:
	MovePlayerPacket() {}
	MovePlayerPacket(int id, float x, float y, float z, float pitch, float yaw): m_id(id), m_x(x), m_y(y), m_z(z), m_pitch(pitch), m_yaw(yaw) {}
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
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
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
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
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
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
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
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
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
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
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int m_x;
	int m_z;
	RakNet::BitStream m_data;
	LevelChunk* m_pChunk;
};

class LevelDataPacket : public Packet
{
public:
	LevelDataPacket() {}
	LevelDataPacket(Level* level) : m_pLevel(level) {}
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	RakNet::BitStream m_data;
	Level* m_pLevel;
};

class PlayerEquipmentPacket : public Packet
{
public:
	PlayerEquipmentPacket() {}
	PlayerEquipmentPacket(int playerID, int itemID): m_playerID(playerID), m_itemID(itemID) {}
	void handle(const RakNet::RakNetGUID&, NetEventCallback* pCallback) override;
	void write(RakNet::BitStream*) override;
	void read(RakNet::BitStream*) override;
public:
	int m_playerID;
	uint16_t m_itemID;
};
