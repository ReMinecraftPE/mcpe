/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Explosion.hpp"

Explosion::Explosion(Level* level, Entity* entity, const Vec3& pos, float power)
{
	field_20 = 0;

	m_bIsFiery = false;

	m_pos = pos;
	m_power = power;
	m_pEntity = entity;
	m_pLevel = level;

	assert(m_power != 0.0f);
}

// This doesn't do a complete explosion, instead, it only adds tiles that should be blown up.
// addParticles() completes the explosion.
void Explosion::explode()
{
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			for (int k = 0; k < 16; k++)
			{
				// what
				if (i != 0 && j != 0 && k != 0 && i != 15 && j != 15 && k != 15)
					continue;

				float rayX = float(i) / 15.0f * 2.0f - 1.0f;
				float rayY = float(j) / 15.0f * 2.0f - 1.0f;
				float rayZ = float(k) / 15.0f * 2.0f - 1.0f;

				// normalize the ray vector
				float length = Mth::sqrt(rayX * rayX + rayY * rayY + rayZ * rayZ);
				rayX /= length;
				rayY /= length;
				rayZ /= length;

				float mult = m_power * (0.7f + 0.6f * m_pLevel->m_random.nextFloat());

				Vec3 pos(m_pos);

				while (true)
				{
					if (mult < 0)
						break;

					TileID tile = m_pLevel->getTile(pos);
					if (tile > 0)
						mult -= 0.3f * (0.3f + Tile::tiles[tile]->getExplosionResistance(m_pEntity));

					if (mult > 0)
						m_tiles.insert(pos);

					mult -= 0.225f;

					if (mult < 0)
						break;

					pos.x += rayX * 0.3f;
					pos.y += rayY * 0.3f;
					pos.z += rayZ * 0.3f;
				}
			}
		}
	}

	m_power *= 2;

	AABB aabb(
		(float)Mth::floor(m_pos.x - m_power - 1.0f),
		(float)Mth::floor(m_pos.y - m_power - 1.0f),
		(float)Mth::floor(m_pos.z - m_power - 1.0f),
		(float)Mth::floor(m_pos.x + m_power + 1.0f),
		(float)Mth::floor(m_pos.y + m_power + 1.0f),
		(float)Mth::floor(m_pos.z + m_power + 1.0f)
	);

	EntityVector ents = m_pLevel->getEntities(m_pEntity, aabb);
	for (int i = 0; i < ents.size(); i++)
	{
		Entity* entity = ents.at(i);
		float distPowerRatio = entity->distanceTo(m_pos) / m_power;
		if (distPowerRatio > 1.0f)
			continue;

		float deltaX = entity->m_pos.x - m_pos.x;
		float deltaY = entity->m_pos.y - m_pos.y;
		float deltaZ = entity->m_pos.z - m_pos.z;

		// @NOTE: They used it here, but not when normalizing the 16*16*16=4096 rays shot before...
		float normInv = Mth::invSqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);
		float hurtPercent = m_pLevel->getSeenPercent(m_pos, entity->m_hitbox) * (1.0f - distPowerRatio);

		entity->hurt(m_pEntity, int((hurtPercent * hurtPercent + hurtPercent) / 2.0f * 8.0f * this->m_power + 1.0f));

		entity->m_vel.x += deltaX * normInv * hurtPercent;
		entity->m_vel.y += deltaY * normInv * hurtPercent;
		entity->m_vel.z += deltaZ * normInv * hurtPercent;
	}

	std::vector<TilePos> vec;
	// @NOTE: Could avoid this copy if m_bFiery is false
	vec.insert(vec.begin(), m_tiles.begin(), m_tiles.end());

	if (!m_bIsFiery)
		return;

	for (int i = int(vec.size() - 1); i >= 0; i--)
	{
		TilePos tp = vec[i];

		TileID tile = m_pLevel->getTile(tp), tileBelow = m_pLevel->getTile(tp.below());
		if (tile == TILE_AIR && Tile::solid[tileBelow] && m_random.nextInt(3) == 0)
			m_pLevel->setTile(tp, Tile::fire->m_ID);
	}
}

void Explosion::addParticles()
{
	m_pLevel->playSound(m_pos, "random.explode", 4.0f, 0.7f * (1.0f + 0.2f * (m_pLevel->m_random.nextFloat() - m_pLevel->m_random.nextFloat())));

	std::vector<TilePos> vec;
	vec.insert(vec.begin(), m_tiles.begin(), m_tiles.end());

	for (int i = int(vec.size() - 1); i >= 0; i--)
	{
		TilePos tp = vec[i];
		TileID tile = m_pLevel->getTile(tp);

		// Spawn a particle only for every 8th tile. Weird
		if ((i & 0x7) == 0)
		{
			float mult;
			Vec3 rp(float(tp.x) + m_pLevel->m_random.nextFloat(),
					float(tp.y) + m_pLevel->m_random.nextFloat(),
					float(tp.z) + m_pLevel->m_random.nextFloat());

			Vec3 d(rp - m_pos);

			// @NOTE: Can use Mth::invSqrt
			// what about .normalize()?
			float dist = d.length();
			Vec3 v = d / dist;

			// @HUH: Dividing by the inverse is the same as multiplying. Thanks, IDA! :)
			float power1 = m_power / (1.0f / dist) + 0.1f;

			mult = ((m_pLevel->m_random.nextFloat() * m_pLevel->m_random.nextFloat()) + 0.3f) * (0.5f / power1);

			m_pLevel->addParticle("explode", Vec3((rp.x + m_pos.x) / 2, (rp.y + m_pos.y) / 2, (rp.z + m_pos.z) / 2), v * mult);
			m_pLevel->addParticle("smoke", rp, v * mult);
		}

		if (tile > 0)
		{
			Tile::tiles[tile]->spawnResources(m_pLevel, tp, m_pLevel->getData(tp), 0.3f);
			m_pLevel->setTile(tp, TILE_AIR);
			Tile::tiles[tile]->wasExploded(m_pLevel, tp);
		}
	}

}
