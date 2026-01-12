/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "MobRenderer.hpp"
#include "client/model/models/HumanoidModel.hpp"

class HumanoidMobRenderer : public MobRenderer
{
public:
	HumanoidMobRenderer(HumanoidModel*, float);
	virtual void additionalRendering(const Mob& mob, float) override;
	virtual void render(const Entity& entity, const Vec3&, float, float) override;
	virtual void onGraphicsReset() override;
	virtual void setupPosition(const Entity& entity, const Vec3& pos, Matrix& matrix) override;
	virtual void setupRotations(const Entity& entity, float bob, float bodyRot, Matrix& matrix, float a) override;

	void renderHand(const Entity& entity, float a);

public:
	HumanoidModel* m_pHumanoidModel;
};
