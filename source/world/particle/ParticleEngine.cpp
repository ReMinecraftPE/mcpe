/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include <sstream>
#include "ParticleEngine.hpp"

ParticleEngine::ParticleEngine(Level* level, Textures* textures)
{
	m_pLevel = level;
	m_pTextures = textures;
	m_pTextures->loadTexture("particles.png", true);
}

void ParticleEngine::setLevel(Level* level)
{
#ifdef ORIGINAL_CODE
	m_pLevel = level;
#endif

	for (int i = 0; i < 4; i++)
	{
#ifndef ORIGINAL_CODE
		for (auto pParticle : m_particles[i])
			delete pParticle;
#endif
		// @BUG: memory leak?
		m_particles[i].clear();
	}

#ifndef ORIGINAL_CODE
	m_pLevel = level;
#endif
}

void ParticleEngine::add(Particle* particle)
{
	m_particles[particle->getParticleTexture()].push_back(particle);
}

std::string ParticleEngine::countParticles()
{
	// @NOTE: For whatever reason this returns a string??
	std::stringstream ss;
	ss << (m_particles[0].size() + m_particles[1].size() + m_particles[2].size() + m_particles[3].size());
	return ss.str();
}

void ParticleEngine::crack(int x, int y, int z, int dir)
{
	TileID tileID = m_pLevel->getTile(x, y, z);
	if (!tileID) return;

	Tile* pTile = Tile::tiles[tileID];

	float posX, posY, posZ;

	switch (dir)
	{
		case DIR_YNEG:
			posX = float(x) + Lerp(pTile->m_aabb.min.x, pTile->m_aabb.max.x - 0.2f, m_random.nextFloat()) + 0.1f;
			posZ = float(z) + Lerp(pTile->m_aabb.min.z, pTile->m_aabb.max.z - 0.2f, m_random.nextFloat()) + 0.1f;
			posY = float(y) + pTile->m_aabb.min.y - 0.1f;
			break;
		case DIR_YPOS:
			posX = float(x) + Lerp(pTile->m_aabb.min.x, pTile->m_aabb.max.x - 0.2f, m_random.nextFloat()) + 0.1f;
			posZ = float(z) + Lerp(pTile->m_aabb.min.z, pTile->m_aabb.max.z - 0.2f, m_random.nextFloat()) + 0.1f;
			posY = float(y) + pTile->m_aabb.max.y + 0.1f;
			break;
		case DIR_ZNEG:
			posX = float(x) + Lerp(pTile->m_aabb.min.x, pTile->m_aabb.max.x - 0.2f, m_random.nextFloat()) + 0.1f;
			posY = float(y) + Lerp(pTile->m_aabb.min.y, pTile->m_aabb.max.y - 0.2f, m_random.nextFloat()) + 0.1f;
			posZ = float(z) + pTile->m_aabb.min.z - 0.1f;
			break;
		case DIR_ZPOS:
			posX = float(x) + Lerp(pTile->m_aabb.min.x, pTile->m_aabb.max.x - 0.2f, m_random.nextFloat()) + 0.1f;
			posY = float(y) + Lerp(pTile->m_aabb.min.y, pTile->m_aabb.max.y - 0.2f, m_random.nextFloat()) + 0.1f;
			posZ = float(z) + pTile->m_aabb.max.z + 0.1f;
			break;
		case DIR_XNEG:
			posY = float(y) + Lerp(pTile->m_aabb.min.y, pTile->m_aabb.max.y - 0.2f, m_random.nextFloat()) + 0.1f;
			posZ = float(z) + Lerp(pTile->m_aabb.min.z, pTile->m_aabb.max.z - 0.2f, m_random.nextFloat()) + 0.1f;
			posX = float(x) + pTile->m_aabb.min.x - 0.1f;
			break;
		case DIR_XPOS:
			posY = float(y) + Lerp(pTile->m_aabb.min.y, pTile->m_aabb.max.y - 0.2f, m_random.nextFloat()) + 0.1f;
			posZ = float(z) + Lerp(pTile->m_aabb.min.z, pTile->m_aabb.max.z - 0.2f, m_random.nextFloat()) + 0.1f;
			posX = float(x) + pTile->m_aabb.max.x + 0.1f;
			break;
		default:
			// @TODO: dont know what they do for the undefined case
			posX = float(x), posY = float(y), posZ = float(z);
			break;
	}

	add((new TerrainParticle(m_pLevel, posX, posY, posZ, pTile))->setPower(0.2f)->scale(0.6f));
}

void ParticleEngine::destroy(int x, int y, int z)
{
	TileID tileID = m_pLevel->getTile(x, y, z);
	if (!tileID) return;

	float timeS = getTimeS();

	Tile* pTile = Tile::tiles[tileID];

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				float posX = float(x) + (float(i) + 0.5f) / 3.0f;
				float posY = float(y) + (float(j) + 0.5f) / 3.0f;
				float posZ = float(z) + (float(k) + 0.5f) / 3.0f;

				add((new TerrainParticle(m_pLevel, posX, posY, posZ, posX - float(x) - 0.5f, posY - float(y) - 0.5f, posZ - float(z) - 0.5f, pTile))->init(x, y, z));
			}
		}
	}

	if (timeS != -1.0)
		getTimeS();

	// @NOTE: Useless string creation
#ifdef ORIGINAL_CODE
	std::string("gen destroy particles: ");
#endif
}

#ifdef ENH_CAMERA_NO_PARTICLES
bool g_bDisableParticles;
#endif

void ParticleEngine::render(Entity* ent, float f)
{
#ifdef ENH_CAMERA_NO_PARTICLES
	if (g_bDisableParticles)
		return;
#endif

	float x1 = Mth::cos(float(M_PI) * ent->m_yaw / 180.0f);
	float x3 = Mth::sin(float(M_PI) * ent->m_yaw / 180.0f);
	float x4 = -(x3 * Mth::sin(float(M_PI) * ent->m_pitch / 180.0f));
	float x5 = x1 * Mth::sin(float(M_PI) * ent->m_pitch / 180.0f);
	float x2 = Mth::cos(float(M_PI) * ent->m_pitch / 180.0f);

	Particle::xOff = Lerp(ent->field_98.x, ent->m_pos.x, f);
	Particle::yOff = Lerp(ent->field_98.y, ent->m_pos.y, f);
	Particle::zOff = Lerp(ent->field_98.z, ent->m_pos.z, f);

	// @BUG: Ignoring the last particle array. Invisible?
	Tesselator& t = Tesselator::instance;
	for (int i = 0; i < 3; i++)
	{
		if (i == PT_TERRAIN)
			m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);
		else
			m_pTextures->loadAndBindTexture("particles.png");

		t.begin();

		for (auto pParticle : m_particles[i])
			pParticle->render(t, f, x1, x2, x3, x4, x5);

		t.draw();
	}
}

void ParticleEngine::renderLit()
{

}

void ParticleEngine::tick()
{
	for (int p = 0; p < 4; p++)
	{
		for (size_t i = 0; i < m_particles[p].size(); i++)
		{
			Particle* particle = m_particles[p][i];
			particle->tick();

			if (particle->m_bRemoved)
			{
				// remove it
				m_particles[p].erase(m_particles[p].begin() + i);
				i--;
				delete particle;
				continue;
			}
		}
	}
}
