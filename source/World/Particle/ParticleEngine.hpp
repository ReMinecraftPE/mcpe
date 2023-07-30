#pragma once

#include "Level.hpp"
#include "Textures.hpp"
#include "Particle.hpp"

class ParticleEngine
{
public:
	ParticleEngine(Level*, Textures*);

	void add(Particle*);
	std::string countParticles();
	void crack(int x, int y, int z, int dir);
	void destroy(int x, int y, int z);
	void render(Entity*, float f);
	void renderLit();
	void tick();
	void setLevel(Level*);

public:
	Level* m_pLevel;
	// todo
	std::vector<Particle*> m_particles[4];
	Textures* m_pTextures;
	Random m_random;
};

