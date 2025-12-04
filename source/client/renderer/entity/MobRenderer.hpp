/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "EntityRenderer.hpp"
#include "client/model/models/Model.hpp"

class MobRenderer : public EntityRenderer
{
public:
	MobRenderer(Model*, float);
    virtual ~MobRenderer();
	void setArmor(Model*);

	virtual void render(const Entity& entity, const Vec3& pos, float, float) override;
	virtual void onGraphicsReset() override;
	virtual int prepareArmor(const Mob& mob, int, float);
#ifdef ENH_GFX_MATRIX_STACK
	virtual void setupPosition(const Entity& entity, const Vec3& pos, Matrix& matrix);
	virtual void setupRotations(const Entity& entity, float bob, float bodyRot, Matrix& matrix, float a);
	virtual void scale(const Mob& mob, Matrix& matrix, float a);
#else
	virtual void setupPosition(const Entity& entity, const Vec3& pos);
	virtual void setupRotations(const Entity& entity, float bob, float bodyRot, float a);
	virtual void scale(const Mob& mob, float a);
#endif
	virtual float getAttackAnim(const Mob& mob, float);
	virtual float getBob(const Mob& mob, float f);
	virtual float getFlipDegrees(const Mob& mob);
	virtual void renderName(const Mob& mob, const Vec3& pos);
	virtual void renderNameTag(const Mob& mob, const std::string&, const Vec3& pos, int);
	virtual void additionalRendering(const Mob& mob, float);

public:
	Model* m_pModel;
	Model* m_pArmorModel;
};

