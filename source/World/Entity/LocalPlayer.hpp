/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Player.hpp"
#include "User.hpp"
#include "KeyboardInput.hpp"

class Minecraft;

class LocalPlayer : public Player
{
public:
	LocalPlayer(Minecraft*, Level*, User*, int);
	virtual ~LocalPlayer();

	// TODO: void addAdditonalSaveData();
	virtual void animateRespawn() override;
	virtual void aiStep() override;
	virtual bool isSneaking() override;
	virtual int move(float x, float y, float z) override;
	virtual void tick() override;
	virtual void updateAi() override;

	void calculateFlight(float, float, float);
	void closeContainer(); //@HUH: oddly enough not a virtual/override


public:
	int field_BEC = 0;
	float field_BF0 = 0.0f;
	float field_BF4 = 0.0f;
	float field_BF8 = 0.0f;
	float field_BFC = 0.0f;
	float field_C00 = 0.0f;
	float field_C04 = 0.0f;
	float field_C08 = 0.0f;
	float field_C0C = 0.0f;
	float field_C10 = 0.0f;
	float field_C14 = 0.0f;
	float field_C18 = 0.0f;
	float field_C1C = 0.0f;
	int m_nAutoJumpFrames = 0;
	// multiplayer related
	float field_C24 = 0.0f;
	float field_C28 = 0.0f;
	float field_C2C = 0.0f;
	float field_C30 = 0.0f;
	float field_C34 = 0.0f;
	// multiplayer related -- end
	int field_C38 = 0;
	Minecraft* m_pMinecraft;
	KeyboardInput* m_pKeyboardInput = nullptr;
};
