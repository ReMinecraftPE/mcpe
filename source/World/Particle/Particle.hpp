/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Entity.hpp"
#include "Tesselator.hpp"

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
public:
	Particle(Level*, float y, float x, float z, float vx, float vy, float vz);

	virtual void render(Tesselator&, float, float, float, float, float, float);
	virtual int getParticleTexture();
	//TODO: addAdditonalSaveData
	//TODO: readAdditionalSaveData
	//TODO: defineSynchedData

	void tick() override;

	Particle* scale(float);
	Particle* setPower(float);

public:
	int field_DC = 0;
	float field_E0 = 0.0f;
	float field_E4 = 0.0f;
	int field_E8 = 0;
	int field_EC = 0;
	float field_F0 = 0.0f;
	float field_F4 = 0.0f;
	float field_F8 = 1.0f;
	float field_FC = 1.0f;
	float field_100 = 1.0f;

	static float xOff, yOff, zOff;
};

class TerrainParticle : public Particle
{
public:
	TerrainParticle(Level*, float x, float y, float z, Tile*);
	TerrainParticle(Level*, float x, float y, float z, float vx, float vy, float vz, Tile*);

	void render(Tesselator&, float, float, float, float, float, float) override;
	int getParticleTexture() override;
	TerrainParticle* init(int x, int y, int z);

public:
	Tile* m_pTile;
};

class BubbleParticle : public Particle
{
public:
	BubbleParticle(Level*, float x, float y, float z, float vx, float vy, float vz);
	void tick() override;
};

class SmokeParticle : public Particle
{
public:
	SmokeParticle(Level*, float x, float y, float z, float vx, float vy, float vz, float a9);
	void tick() override;
	void render(Tesselator&, float, float, float, float, float, float) override;

public:
	float field_104 = 0.0f;
};

class RedDustParticle : public Particle
{
public:
	RedDustParticle(Level*, float x, float y, float z, float vx, float vy, float vz);
	void tick() override;
	void render(Tesselator&, float, float, float, float, float, float) override;

public:
	float field_104 = 0.0f;
};

class ExplodeParticle : public Particle
{
public:
	ExplodeParticle(Level*, float x, float y, float z, float vx, float vy, float vz);
	void tick() override;

public:
	float field_104 = 0.0f;
};

class FlameParticle : public Particle
{
public:
	FlameParticle(Level*, float x, float y, float z, float vx, float vy, float vz);
	void tick() override;
	void render(Tesselator&, float, float, float, float, float, float) override;
	float getBrightness(float f) override;

public:
	float field_104 = 0.0f;
};

class LavaParticle : public Particle
{
public:
	LavaParticle(Level*, float x, float y, float z);
	void tick() override;
	void render(Tesselator&, float, float, float, float, float, float) override;
	float getBrightness(float f) override;

public:
	float field_104 = 0.0f;
};
