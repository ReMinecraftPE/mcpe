/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "RakNetTypes.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"

#define NETWORK_PROTOCOL_VERSION_MIN 3 // 3 because the packet IDs changed completely between 2 and 3
//#define NETWORK_PROTOCOL_VERSION 2   // 0.1.0 (actual client crashes with unrecognized tiles)
#define NETWORK_PROTOCOL_VERSION 3	   // 0.2.0

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
	PACKET_EXPLODE, // @TODO
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
	virtual void write(RakNet::BitStream&) = 0;
	virtual void read(RakNet::BitStream&) = 0;
	virtual void handle(const RakNet::RakNetGUID&, NetEventCallback&) = 0;
};