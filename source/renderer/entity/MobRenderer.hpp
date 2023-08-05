/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "EntityRenderer.hpp"

class MobRenderer : public EntityRenderer
{
public:
	MobRenderer(Model*, float);
	void setArmor(Model*);

	virtual void render(Entity*, float, float, float, float, float) override;
	virtual int prepareArmor(Mob*, int, float);
	virtual void setupPosition(Entity*, float, float, float);
	virtual void setupRotations(Entity*, float, float, float);
	virtual float getAttackAnim(Mob*, float);
	virtual float getBob(Mob*, float);
	virtual float getFlipDegrees(Mob*);
	virtual int getOverlayColor(Mob*, float, float);
	virtual void scale(Mob*, float);
	virtual void renderName(Mob*, float, float, float);
	virtual void renderNameTag(Mob*, const std::string&, float, float, float, int);
	virtual void additionalRendering(Mob*, float);

public:
	Model* m_pModel;
	Model* m_pArmorModel = nullptr;
};

