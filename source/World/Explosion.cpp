/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Explosion.hpp"

Explosion::Explosion(Level* level, Entity* entity, float x, float y, float z, float power)
{
	m_pos = Vec3(x, y, z);
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
				float length = sqrtf(rayX * rayX + rayY * rayY + rayZ * rayZ);
				rayX /= length;
				rayY /= length;
				rayZ /= length;

				float mult = m_power * (0.7f + 0.6f * m_pLevel->field_38.nextFloat());

				float posX = m_pos.x;
				float posY = m_pos.y;
				float posZ = m_pos.z;

				while (true)
				{
					if (mult < 0)
						break;

					int pxf = Mth::floor(posX);
					int pyf = Mth::floor(posY);
					int pzf = Mth::floor(posZ);

					TileID tile = m_pLevel->getTile(pxf, pyf, pzf);
					if (tile > 0)
						mult -= 0.3f * (0.3f + Tile::tiles[tile]->getExplosionResistance(m_pEntity));

					if (mult > 0)
						m_tiles.insert(TilePos(pxf, pyf, pzf));

					mult -= 0.225f;
					
					if (mult < 0)
						break;

					posX += rayX * 0.3f;
					posY += rayY * 0.3f;
					posZ += rayZ * 0.3f;
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

	EntityVector* pEnts = m_pLevel->getEntities(m_pEntity, aabb);
	for (Entity* pEnt : (*pEnts))
	{
		float distPowerRatio = pEnt->distanceTo(m_pos.x, m_pos.y, m_pos.z) / m_power;
		if (distPowerRatio > 1.0f)
			continue;

		float deltaX = pEnt->m_pos.x - m_pos.x;
		float deltaY = pEnt->m_pos.y - m_pos.y;
		float deltaZ = pEnt->m_pos.z - m_pos.z;

		// @NOTE: They used it here, but not when normalizing the 16*16*16=4096 rays shot before...
		float normInv = Mth::invSqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);
		float hurtPercent = m_pLevel->getSeenPercent(m_pos, pEnt->m_hitbox) * (1.0f - distPowerRatio);

		pEnt->hurt(m_pEntity, int((hurtPercent * hurtPercent + hurtPercent) / 2.0f * 8.0f * this->m_power + 1.0f));

		pEnt->m_vel.x += deltaX * normInv * hurtPercent;
		pEnt->m_vel.y += deltaY * normInv * hurtPercent;
		pEnt->m_vel.z += deltaZ * normInv * hurtPercent;
	}

	std::vector<TilePos> vec;
	// @NOTE: Could avoid this copy if m_bFiery is false
	vec.insert(vec.begin(), m_tiles.begin(), m_tiles.end());

	if (!m_bIsFiery)
		return;

	for (int i = int(vec.size() - 1); i >= 0; i--)
	{
		TilePos tp = vec[i];

		TileID tile = m_pLevel->getTile(tp.x, tp.y, tp.z), tileBelow = m_pLevel->getTile(tp.x, tp.y - 1, tp.z);
		if (tile == TILE_AIR && Tile::solid[tileBelow] && m_random.nextInt(3) == 0)
			m_pLevel->setTile(tp.x, tp.y, tp.z, Tile::fire->m_ID);
	}
}

void Explosion::addParticles()
{
	m_pLevel->playSound(m_pos.x, m_pos.y, m_pos.z, "random.explode", 4.0f, 0.7f * (1.0f + 0.2f * (m_pLevel->field_38.nextFloat() - m_pLevel->field_38.nextFloat())));

	std::vector<TilePos> vec;
	vec.insert(vec.begin(), m_tiles.begin(), m_tiles.end());

	for (int i = int(vec.size() - 1); i >= 0; i--)
	{
		TilePos tp = vec[i];
		TileID tile = m_pLevel->getTile(tp.x, tp.y, tp.z);

		// Spawn a particle only for every 8th tile. Weird
		if ((i & 0x7) == 0)
		{
			float x2, y2, z2, vx, vy, vz, mult;
			x2 = float(tp.x) + m_pLevel->field_38.nextFloat();
			y2 = float(tp.y) + m_pLevel->field_38.nextFloat();
			z2 = float(tp.z) + m_pLevel->field_38.nextFloat();

			float dX, dY, dZ;
			dX = x2 - m_pos.x;
			dY = y2 - m_pos.y;
			dZ = z2 - m_pos.z;

			// @NOTE: Can use Mth::invSqrt
			float dist = Mth::sqrt(dX * dX + dY * dY + dZ * dZ);
			vx = dX / dist;
			vy = dY / dist;
			vz = dZ / dist;

			// @HUH: Dividing by the inverse is the same as multiplying. Thanks, IDA! :)
			float power1 = m_power / (1.0f / dist) + 0.1f;

			mult = ((m_pLevel->field_38.nextFloat() * m_pLevel->field_38.nextFloat()) + 0.3f) * (0.5f / power1);

			m_pLevel->addParticle("explode", (x2 + m_pos.x) / 2, (y2 + m_pos.y) / 2, (z2 + m_pos.z) / 2, vx * mult, vy * mult, vz * mult);
			m_pLevel->addParticle("smoke", x2, y2, z2, vx * mult, vy * mult, vz * mult);
		}

		if (tile > 0)
		{
			Tile::tiles[tile]->spawnResources(m_pLevel, tp.x, tp.y, tp.z, m_pLevel->getData(tp.x, tp.y, tp.z), 0.3f);
			m_pLevel->setTile(tp.x, tp.y, tp.z, TILE_AIR);
			Tile::tiles[tile]->wasExploded(m_pLevel, tp.x, tp.y, tp.z);
		}
	}

}
