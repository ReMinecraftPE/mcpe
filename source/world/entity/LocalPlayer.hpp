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

	// TODO: void addAdditonalSaveData();
	virtual void animateRespawn() override;
	virtual void aiStep() override;
	virtual bool isSneaking() override;
	virtual int move(float x, float y, float z) override;
	virtual void tick() override;
	virtual void updateAi() override;
	virtual bool isLocalPlayer() override;

	void calculateFlight(float, float, float);
	void closeContainer(); //@HUH: oddly enough not a virtual/override
	void respawn();

public:
	int field_BEC;
	float field_BF0;
	float field_BF4;
	float field_BF8;
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
	// multiplayer related
	float field_C24;
	float field_C28;
	float field_C2C;
	float field_C30;
	float field_C34;
	// multiplayer related -- end
	int field_C38;
	Minecraft* m_pMinecraft;
	IMoveInput* m_pMoveInput;
	float m_renderArmYaw;
	float m_renderArmPitch;
	float m_lastRenderArmYaw;
	float m_lastRenderArmPitch;
};
