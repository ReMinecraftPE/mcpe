/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Packet.hpp"
#include "packets/LoginPacket.hpp"
#include "packets/LoginStatusPacket.hpp"
#include "packets/ReadyPacket.hpp"
#include "packets/MessagePacket.hpp"
#include "packets/SetTimePacket.hpp"
#include "packets/StartGamePacket.hpp"
#include "packets/AddMobPacket.hpp"
#include "packets/AddPlayerPacket.hpp"
#include "packets/RemoveEntityPacket.hpp"
#include "packets/AddItemEntityPacket.hpp"
#include "packets/TakeItemEntityPacket.hpp"
#include "packets/MoveEntityPacket.hpp"
#include "packets/MoveEntityPacket_PosRot.hpp"
#include "packets/MovePlayerPacket.hpp"
#include "packets/PlaceBlockPacket.hpp"
#include "packets/RemoveBlockPacket.hpp"
#include "packets/UpdateBlockPacket.hpp"
#include "packets/LevelEventPacket.hpp"
#include "packets/EntityEventPacket.hpp"
#include "packets/RequestChunkPacket.hpp"
#include "packets/ChunkDataPacket.hpp"
#include "packets/PlayerEquipmentPacket.hpp"
#include "packets/InteractPacket.hpp"
#include "packets/UseItemPacket.hpp"
#include "packets/SetEntityDataPacket.hpp"
#include "packets/SetHealthPacket.hpp"
#include "packets/AnimatePacket.hpp"
#include "packets/RespawnPacket.hpp"
#include "packets/LevelDataPacket.hpp"

class MinecraftPackets
{
public:
	static Packet* createPacket(int type);
};

