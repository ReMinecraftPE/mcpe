/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Entity.hpp"

#define C_MAX_MOB_HEALTH (20)

class Mob : public Entity
{
public:
	Mob(Level* pLevel);
	virtual ~Mob();

	//overrides
	virtual void lerpTo(float x, float y, float z, float yaw, float pitch, int i) override;
	virtual void tick() override;
	virtual void baseTick() override;
	virtual float getHeadHeight() override;
	virtual bool isPickable() override;
	virtual bool isPushable() override;
	virtual bool isShootable() override;
	virtual bool isAlive() override;
	virtual bool hurt(Entity*, int) override;
	virtual void animateHurt() override;
	virtual void setSize(float rad, float height) override;
	virtual void outOfWorld() override;
	virtual void causeFallDamage(float level) override;

	//virtuals
	virtual void knockback(Entity* pEnt, int a, float x, float z);
	virtual void die(Entity* pCulprit);
	virtual bool canSee(Entity* pEnt);
	virtual bool onLadder();
	virtual void spawnAnim();
	virtual std::string getTexture();
	virtual bool isWaterMob();
	virtual void playAmbientSound();
	virtual int getAmbientSoundInterval();
	virtual void superTick();
	virtual void heal(int health);
	virtual HitResult pick(float, float);
	virtual void travel(float, float);
	virtual void updateWalkAnim();
	virtual void aiStep();
	//AddAdditonalSaveData TODO
	//ReadAdditionalSaveData TODO
	virtual void lookAt(Entity* pEnt, float, float);
	virtual bool isLookingAtAnEntity();
	virtual Entity* getLookingAt();
	virtual void beforeRemove();
	virtual bool canSpawn();
	virtual float getAttackAnim(float f);
	virtual Vec3 getPos(float f);
	virtual Vec3 getLookAngle(float f);
	virtual Vec3 getViewVector(float f);
	virtual int getMaxSpawnClusterSize();
	virtual void actuallyHurt(int damage);
	virtual bool removeWhenFarAway();
	virtual int getDeathLoot();
	virtual void dropDeathLoot();
	virtual bool isImmobile();
	virtual void jumpFromGround();
	virtual void updateAi();
	virtual int getMaxHeadXRot();
	virtual float getSoundVolume();
	virtual std::string getAmbientSound();
	virtual std::string getHurtSound();
	virtual std::string getDeathSound();
	virtual void defineSynchedData();

	float rotlerp(float, float, float);

public:
	int field_DC = 10;
	float field_E0;
	float field_E4;
	float field_E8 = 0.0f;
	float field_EC = 0.0f;
	char field_F0 = 0;
	float field_F4 = 0.0f;
	float field_F8 = 0.0f;
	int m_health = 10;
	int field_100 = 20;
	int field_104 = 0;
	int field_108 = 0;
	float field_10C = 0.0f;
	int field_110 = 0;
	int field_114 = 0;
	float field_118 = 0.0f;
	float field_11C = 0.0f;
	int field_120 = 0;
	int field_124 = 0;
	float field_128 = 0.0f;
	float field_12C = 0.0f;
	float field_130 = 0.0f;
	Random m_random;
	int field_AFC = 0;
	float field_B00 = 0.0f;
	float field_B04 = 0.0f;
	float field_B08 = 0.0f;
	char field_B0C = 0;
	int field_B10 = 0;
	float field_B14 = 0.7f;
	std::string m_texture;
	std::string m_class;
	int field_B48 = 0;
	float field_B4C = 0.0f;
	float field_B50 = 0.0f;
	float field_B54 = 0.0f;
	float field_B58 = 0.0f;
	float field_B5C = 0.0f;
	float field_B60 = 1.0f;
	int field_B64 = 0;
	char field_B68 = 1;
	char field_B69 = 0;
	int field_B6C = 0;
	float field_B70 = 0.0f;
	float field_B74 = 0.0f;
	float field_B78 = 0.0f;
	float field_B7C = 0.0f;
	float field_B80 = 0.0f;
	int field_B84 = 0;
	Entity* m_pEntLookedAt = nullptr;
};
