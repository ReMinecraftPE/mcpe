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

	virtual void render(Entity*, const Vec3& pos, float, float) override;
	virtual void onGraphicsReset() override;
	virtual int prepareArmor(Mob*, int, float);
#ifdef ENH_GFX_MATRIX_STACK
	virtual void setupPosition(Entity*, const Vec3& pos, Matrix& matrix);
	virtual void setupRotations(Entity*, float bob, float bodyRot, Matrix& matrix, float a);
	virtual void scale(Mob* pMob, Matrix& matrix, float a);
#else
	virtual void setupPosition(Entity*, const Vec3& pos);
	virtual void setupRotations(Entity*, float bob, float bodyRot, float a);
	virtual void scale(Mob* pMob, float a);
#endif
	virtual float getAttackAnim(Mob*, float);
	virtual float getBob(Mob*, float);
	virtual float getFlipDegrees(Mob*);
	virtual int getOverlayColor(Mob*, float, float);
	virtual void renderName(Mob*, const Vec3& pos);
	virtual void renderNameTag(Mob*, const std::string&, const Vec3& pos, int);
	virtual void additionalRendering(Mob*, float);

public:
	Model* m_pModel;
	Model* m_pArmorModel;
};

