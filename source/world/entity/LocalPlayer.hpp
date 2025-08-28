/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "world/entity/Player.hpp"
#include "client/player/input/IMoveInput.hpp"
#include "client/player/input/User.hpp"

class Minecraft;

class LocalPlayer : public Player
{
public:
	LocalPlayer(Minecraft*, Level*, User*, GameType, int);
	virtual ~LocalPlayer();

	virtual void animateRespawn() override;
	virtual void aiStep() override;
	virtual bool isSneaking() const override;
	virtual int move(const Vec3& pos) override;
	virtual void tick() override;
	virtual void updateAi() override;
	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;
	virtual bool isLocalPlayer() const override { return true; }
	virtual void drop(const ItemInstance& item, bool randomly = false) override;
	virtual bool isImmobile() const override;
	virtual void setPlayerGameType(GameType gameType) override;

	void calculateFlight(const Vec3& pos);
	void closeContainer(); //@HUH: oddly enough not a virtual/override
	void respawn();

private:
	// Made these private since they're only accessed by LocalPlayer
	// multiplayer related
	Vec3 field_C24;
	Vec2 field_C30;
	// multiplayer related -- end

public:
	int field_BEC;
	Vec3 field_BF0;
	float field_BFC;
	float field_C00;
	float field_C04;
	float field_C08;
	float field_C0C;
	float field_C10;
	float field_C14;
	float field_C18;
	float field_C1C;
	int m_nAutoJumpFrames;
	int field_C38;
	Minecraft* m_pMinecraft;
	IMoveInput* m_pMoveInput;
	Vec2 m_renderArmRot;
	Vec2 m_lastRenderArmRot;
};
