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
		for (std::vector<Particle*>::iterator it = m_particles[i].begin(); it != m_particles[i].end(); it++)
		{
			Particle* pParticle = *it;
			delete pParticle;
		}
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

void ParticleEngine::crack(const TilePos& tilePos, Facing::Name face)
{
	TileID tileID = m_pLevel->getTile(tilePos);
	if (!tileID) return;

	Tile* pTile = Tile::tiles[tileID];

	Vec3 pos(tilePos);

	switch (face)
	{
		case Facing::DOWN:
			pos.x += Mth::Lerp(pTile->m_aabb.min.x, pTile->m_aabb.max.x - 0.2f, m_random.nextFloat()) + 0.1f;
			pos.z += Mth::Lerp(pTile->m_aabb.min.z, pTile->m_aabb.max.z - 0.2f, m_random.nextFloat()) + 0.1f;
			pos.y += pTile->m_aabb.min.y - 0.1f;
			break;
		case Facing::UP:
			pos.x += Mth::Lerp(pTile->m_aabb.min.x, pTile->m_aabb.max.x - 0.2f, m_random.nextFloat()) + 0.1f;
			pos.z += Mth::Lerp(pTile->m_aabb.min.z, pTile->m_aabb.max.z - 0.2f, m_random.nextFloat()) + 0.1f;
			pos.y += pTile->m_aabb.max.y + 0.1f;
			break;
		case Facing::NORTH:
			pos.x += Mth::Lerp(pTile->m_aabb.min.x, pTile->m_aabb.max.x - 0.2f, m_random.nextFloat()) + 0.1f;
			pos.y += Mth::Lerp(pTile->m_aabb.min.y, pTile->m_aabb.max.y - 0.2f, m_random.nextFloat()) + 0.1f;
			pos.z += pTile->m_aabb.min.z - 0.1f;
			break;
		case Facing::SOUTH:
			pos.x += Mth::Lerp(pTile->m_aabb.min.x, pTile->m_aabb.max.x - 0.2f, m_random.nextFloat()) + 0.1f;
			pos.y += Mth::Lerp(pTile->m_aabb.min.y, pTile->m_aabb.max.y - 0.2f, m_random.nextFloat()) + 0.1f;
			pos.z += pTile->m_aabb.max.z + 0.1f;
			break;
		case Facing::WEST:
			pos.y += Mth::Lerp(pTile->m_aabb.min.y, pTile->m_aabb.max.y - 0.2f, m_random.nextFloat()) + 0.1f;
			pos.z += Mth::Lerp(pTile->m_aabb.min.z, pTile->m_aabb.max.z - 0.2f, m_random.nextFloat()) + 0.1f;
			pos.x += pTile->m_aabb.min.x - 0.1f;
			break;
		case Facing::EAST:
			pos.y += Mth::Lerp(pTile->m_aabb.min.y, pTile->m_aabb.max.y - 0.2f, m_random.nextFloat()) + 0.1f;
			pos.z += Mth::Lerp(pTile->m_aabb.min.z, pTile->m_aabb.max.z - 0.2f, m_random.nextFloat()) + 0.1f;
			pos.x += pTile->m_aabb.max.x + 0.1f;
			break;
		default:
			// @TODO: dont know what they do for the undefined case
			break;
	}

	add((new TerrainParticle(m_pLevel, pos, pTile))->init(tilePos, face)->setPower(0.2f)->scale(0.6f));
}

void ParticleEngine::destroyEffect(const TilePos& pos)
{
	TileID tileID = m_pLevel->getTile(pos);
	if (!tileID) return;

	float timeS = getTimeS();

	Tile* pTile = Tile::tiles[tileID];

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				Vec3 vec1(float(pos.x) + (float(i) + 0.5f) / 3.0f,
					     float(pos.y) + (float(j) + 0.5f) / 3.0f,
					     float(pos.z) + (float(k) + 0.5f) / 3.0f);
				Vec3 vec2(vec1.x - float(pos.x) - 0.5f,
					      vec1.y - float(pos.y) - 0.5f,
					      vec1.z - float(pos.z) - 0.5f);

				add((new TerrainParticle(m_pLevel, vec1, vec2, pTile))->init(pos));
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

	float x1 = Mth::cos(float(M_PI) * ent->m_rot.x / 180.0f);
	float x3 = Mth::sin(float(M_PI) * ent->m_rot.x / 180.0f);
	float x4 = -(x3 * Mth::sin(float(M_PI) * ent->m_rot.y / 180.0f));
	float x5 = x1 * Mth::sin(float(M_PI) * ent->m_rot.y / 180.0f);
	float x2 = Mth::cos(float(M_PI) * ent->m_rot.y / 180.0f);

	Particle::xOff = Mth::Lerp(ent->m_posPrev.x, ent->m_pos.x, f);
	Particle::yOff = Mth::Lerp(ent->m_posPrev.y, ent->m_pos.y, f);
	Particle::zOff = Mth::Lerp(ent->m_posPrev.z, ent->m_pos.z, f);

	// @BUG: Ignoring the last particle array. Invisible?
	Tesselator& t = Tesselator::instance;
	for (int i = 0; i < 3; i++)
	{
		if (i == PT_TERRAIN)
			m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);
		else
			m_pTextures->loadAndBindTexture("particles.png");

		t.begin();

		for (std::vector<Particle*>::iterator it = m_particles[i].begin(); it != m_particles[i].end(); it++)
		{
			Particle* pParticle = *it;
			pParticle->render(t, f, x1, x2, x3, x4, x5);
		}

		t.draw();
	}
}

void ParticleEngine::renderLit(Entity* player, float a)
{
	int tt = 3;
	if (m_particles[tt].size() != 0)
	{
		Tesselator& t = Tesselator::instance;

		for (int i = 0; i < m_particles[tt].size(); ++i)
		{
			Particle* p = m_particles[tt].at(i);
			p->render(t, a, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		}

	}
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
