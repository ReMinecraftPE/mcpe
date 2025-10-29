/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "client/player/LocalPlayer.hpp"
#include "world/level/Level.hpp"
#include "world/item/ItemInstance.hpp"

class Minecraft;

class GameMode
{
protected:
	Level& _level;

public:
	GameMode(Minecraft* pMinecraft, Level& level);
	virtual ~GameMode();
	virtual void initLevel(Level*);
	//virtual bool isDestroyingBlock() const;
	virtual bool startDestroyBlock(Player* player, const TilePos& pos, Facing::Name face);
	virtual bool destroyBlock(Player* player, const TilePos& pos, Facing::Name face);
	virtual bool continueDestroyBlock(Player* player, const TilePos& pos, Facing::Name face);
	virtual void stopDestroyBlock();
	virtual void tick();
	virtual void render(float f);
	// Used to be called getPickRange
	virtual float getBlockReachDistance() const;
	virtual float getEntityReachDistance() const;
	virtual bool useItem(Player*, Level*, ItemInstance*);
	virtual bool useItemOn(Player*, Level*, ItemInstance*, const TilePos& pos, Facing::Name face);
	virtual LocalPlayer* createPlayer(Level*);
	virtual void initPlayer(Player*);
	virtual void adjustPlayer(Player*);
	virtual bool canHurtPlayer();
	virtual void interact(Player*, Entity*);
	virtual void attack(Player*, Entity*);
	virtual int handleInventoryMouseClick(int, int, int, Player*);
	virtual void handleCloseInventory(int, Player*);
	virtual bool isCreativeType() const { return true; }
	virtual bool isSurvivalType() const { return false; }
	virtual float getDestroyModifier() const { return 1.0; }

public:
	Minecraft* m_pMinecraft;
	uint8_t field_8;
};
