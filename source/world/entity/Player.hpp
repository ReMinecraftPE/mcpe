/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "thirdparty/raknet/RakNetTypes.h"
#include "world/item/Inventory.hpp"
#include "world/entity/Mob.hpp"
#include "world/entity/ItemEntity.hpp"
#include "world/gamemode/GameType.hpp"

class Inventory; // in case we're included from Inventory.hpp

class Player : public Mob
{
private:
	GameType _playerGameType;

private:
	void _init();

public:
	Player(Level* pLevel, GameType gameType);
	virtual ~Player();

protected:
	virtual void reallyDrop(ItemEntity* pEnt);
	bool getSharedFlag(SharedFlag flag) const override { return false; }
	void setSharedFlag(SharedFlag flag, bool value) override {}

public:
	void reset() override;
	void remove() override;
	float getHeadHeight() const override { return 0.12f; /*@HUH: what ?*/ }
	int getMaxHealth() const override { return 20; }
	bool isShootable() const override { return true; }
	bool isPlayer() const override { return true; }
	bool isCreativeModeAllowed() const override { return true; }
	bool hurt(Entity*, int) override;
	void awardKillScore(Entity* pKilled, int score) override;
	void resetPos(bool respawn = false) override;
	void die(Entity* pCulprit) override;
	void aiStep() override;
	ItemInstance* getCarriedItem() override;
	bool isImmobile() const override { return m_health <= 0; }
	void updateAi() override;
	void addAdditionalSaveData(CompoundTag& tag) const override;
	void readAdditionalSaveData(const CompoundTag& tag) override;

	virtual void animateRespawn();
	virtual void drop();
	virtual void drop(const ItemInstance& item, bool randomly = false);
	virtual void startCrafting(const TilePos& pos);
	virtual void startStonecutting(const TilePos& pos);
	virtual void startDestroying();
	virtual void stopDestroying();
	virtual bool isLocalPlayer() const { return false; }
	virtual void take(Entity* pEnt, int count) {}

	int addResource(int);
	void animateRespawn(Player*, Level*);
	void attack(Entity* pEnt);
	void useItem(ItemInstance& item) const;
	bool canDestroy(const Tile*) const;
	void closeContainer();
	void displayClientMessage(const std::string& msg);
	float getDestroySpeed() const { return 1.0f; }
	int getInventorySlot(int x) const;
	TilePos getRespawnPosition() const { return m_respawnPos; }
	int getScore() const { return m_score; }
	void prepareCustomTextures();
	void respawn();
	void rideTick();
	void setDefaultHeadHeight();
	void setRespawnPos(const TilePos& pos);

	void touch(Entity* pEnt);
	GameType getPlayerGameType() const { return _playerGameType; }
	virtual void setPlayerGameType(GameType playerGameType) { _playerGameType = playerGameType; }
	bool isSurvival() const { return getPlayerGameType() == GAME_TYPE_SURVIVAL; }
	bool isCreative() const { return getPlayerGameType() == GAME_TYPE_CREATIVE; }
	ItemInstance* getSelectedItem() const;
	void removeSelectedItem();
	bool isUsingItem() const { return !ItemInstance::isNull(getSelectedItem()); }

	// QUIRK: Yes, I did mean it like that, as did Mojang.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
	void interact(Entity* pEnt);
#pragma GCC diagnostic pop

public:
	//TODO
	Inventory* m_pInventory;
	uint8_t field_B94;
	int m_score;
	float m_oBob; // field_B9C
	float m_bob;
	std::string m_name;
	int m_dimension;
	RakNet::RakNetGUID m_guid;
	//TODO
	TilePos m_respawnPos;
	//TODO
	bool m_bHasRespawnPos;
	//TODO
	bool m_destroyingBlock;
};

