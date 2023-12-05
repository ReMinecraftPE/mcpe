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
	virtual void reset() override;
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
	virtual bool isBaby();
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
	int field_DC;
	float field_E0;
	float field_E4;
	float field_E8;
	float field_EC;
	char field_F0;
	float field_F4;
	float field_F8;
	int m_health;
	int field_100;
	int field_104;
	int field_108;
	float field_10C;
	int field_110;
	int field_114;
	float field_118;
	float field_11C;
	int field_120;
	int field_124;
	float field_128;
	float field_12C;
	float field_130;
	Random m_random;
	int field_AFC;
	float field_B00;
	float field_B04;
	float field_B08;
	char field_B0C;
	float field_B10;
	float field_B14;
	std::string m_texture;
	std::string m_class;
	int field_B48;
	float field_B4C;
	float field_B50;
	float field_B54;
	float field_B58;
	float field_B5C;
	float field_B60;
	int field_B64;
	char field_B68;
	char field_B69;
	int field_B6C;
	float field_B70;
	float field_B74;
	float field_B78;
	float field_B7C;
	float field_B80;
	int field_B84;
	Entity* m_pEntLookedAt;

	float v020_field_104;
};
