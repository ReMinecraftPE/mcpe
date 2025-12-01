/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "world/level/Level.hpp"
#include "client/renderer/Textures.hpp"
#include "Particle.hpp"

class ParticleEngine
{
public:
	ParticleEngine(Level*, Textures*);

	void add(Particle*);
	std::string countParticles();
	void crack(const TilePos& tilePos, Facing::Name face);
	void destroyEffect(const TilePos& pos);
	void render(const Entity& camera, float f);
	void renderLit(const Entity& camera, float a);
	void tick();
	void setLevel(Level*);

public:
	Level* m_pLevel;
	// todo
	std::vector<Particle*> m_particles[4];
	Textures* m_pTextures;
	Random m_random;
};

