/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Mob.hpp"
#include "Inventory.hpp"
#include "ItemEntity.hpp"
#include "RakNetTypes.h"

class Inventory; // in case we're included from Inventory.hpp

class Player : public Mob
{
public:
	Player(Level*);
	virtual ~Player();

	virtual void remove() override;
	virtual void tick() override;
	virtual bool isInWall() override;
	virtual float getHeadHeight() override;
	virtual bool isShootable() override;
	virtual bool isPlayer() override;
	virtual bool isCreativeModeAllowed() override;
	virtual bool hurt(Entity*, int) override;
	virtual void awardKillScore(Entity* pKilled, int score) override;
	virtual void resetPos() override;
	virtual void die(Entity* pCulprit) override;
	virtual void aiStep() override;
	virtual bool isImmobile() override;
	virtual void updateAi() override;
	virtual void defineSynchedData() override;

	virtual void animateRespawn();

	int addResource(int);
	void animateRespawn(Player*, Level*);
	void attack(Entity* pEnt);
	bool canDestroy(Tile*);
	void closeContainer();
	void displayClientMessage(const std::string& msg);
	void drop(ItemInstance*);
	void drop(ItemInstance*, bool);
	void drop();
	float getDestroySpeed();
	int getInventorySlot(int x);
	Pos getRespawnPosition();
	int getScore();
	void prepareCustomTextures();
	void reallyDrop(ItemEntity* pEnt);
	void respawn();
	void rideTick();
	void setDefaultHeadHeight();
	void setRespawnPos(Pos*);
	void startCrafting(int, int, int);
	void swing();
	void take(Entity* pEnt, int x);
	void touch(Entity* pEnt);
	void interact(Entity* pEnt);

public:
	//TODO
	Inventory* m_pInventory = nullptr;
	uint8_t field_B94 = 0;
	int m_score = 0;
	float field_B9C = 0.0f;
	float field_BA0 = 0.0f;
	bool field_BA4 = false;
	int field_BA8 = 0;
	std::string m_name = "";
	int field_BC4 = 0;
	RakNet::RakNetGUID m_guid;
	//TODO
	Pos m_respawnPos;
	//TODO
	bool m_bHaveRespawnPos = false;
	//TODO
};

