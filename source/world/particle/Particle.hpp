/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../entity/Entity.hpp"
#include "client/renderer/Tesselator.hpp"

enum eParticleTexture
{
	PT_PARTICLES,
	PT_TERRAIN,
	PT_PARTICLES2,
	PT_PARTICLES3,
};

enum eParticleTextureIndex
{
	PTI_BUBBLE = 32,
	PTI_FLAME = 48,
	PTI_LAVA,
};

class Particle : public Entity
{
private:
	void _init();
public:
	Particle() { _init(); }
	Particle(Level*, const Vec3& pos, const Vec3& dir);

	virtual void render(Tesselator&, float, float, float, float, float, float);
	virtual int getParticleTexture();

	void tick() override;
	void addAdditionalSaveData(CompoundTag& tag) const override {}
	void readAdditionalSaveData(const CompoundTag& tag) override {}

	Particle* scale(float);
	Particle* setPower(float);

public:
	int field_DC;
	float field_E0;
	float field_E4;
	int field_E8;
	int field_EC;
	float field_F0;
	float field_F4;
	float m_rCol;
	float m_gCol;
	float m_bCol;
	bool m_bIsUnlit;

	static float xOff, yOff, zOff;
};

class TerrainParticle : public Particle
{
private:
	void _init(Tile* tile);
public:
	TerrainParticle(Level*, const Vec3& pos, Tile*);
	TerrainParticle(Level*, const Vec3& pos, const Vec3& dir, Tile*);

	void render(Tesselator&, float, float, float, float, float, float) override;
	int getParticleTexture() override;
	TerrainParticle* init(const TilePos& tilePos, Facing::Name face = Facing::DOWN);

public:
	Tile* m_pTile;
};

class BubbleParticle : public Particle
{
public:
	BubbleParticle(Level*, const Vec3& pos, const Vec3& dir);
	void tick() override;
};

class SmokeParticle : public Particle
{
public:
	SmokeParticle(Level*, const Vec3& pos, const Vec3& dir, float a9);
	void tick() override;
	void render(Tesselator&, float, float, float, float, float, float) override;

public:
	float field_104;
};

class RedDustParticle : public Particle
{
public:
	RedDustParticle(Level*, const Vec3& pos, const Vec3& dir);
	void tick() override;
	void render(Tesselator&, float, float, float, float, float, float) override;

public:
	float field_104;
};

class ExplodeParticle : public Particle
{
public:
	ExplodeParticle(Level*, const Vec3& pos, const Vec3& dir);
	void tick() override;

public:
	float field_104;
};

class FlameParticle : public Particle
{
public:
	FlameParticle(Level*, const Vec3& pos, const Vec3& dir);
	void tick() override;
	void render(Tesselator&, float, float, float, float, float, float) override;
	float getBrightness(float f) const override;

public:
	float field_104;
};

class LavaParticle : public Particle
{
public:
	LavaParticle(Level*, const Vec3& pos);
	void tick() override;
	void render(Tesselator&, float, float, float, float, float, float) override;
	float getBrightness(float f) const override;

public:
	float field_104;
};
