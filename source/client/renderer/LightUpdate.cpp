#include "client/renderer/LightUpdate.hpp"
#include "world/level/TileSource.hpp"
#include "world/level/levelgen/chunk/ChunkConstants.hpp"

void LightUpdate::update()
{
	if (!m_source)
		return;

	// clamp y values to 0-127 for min and max
	if (m_min.y < 0)
		m_min.y = 0;
	if (m_max.y > (ChunkConstants::Y_SIZE - 1))
		m_max.y = ChunkConstants::Y_SIZE - 1;

	TilePos pos;
	for (pos.x = m_min.x; pos.x <= m_max.x; pos.x++)
	{
		for (pos.z = m_min.z; pos.z <= m_max.z; pos.z++)
		{
			pos.y = m_min.y;
			if (!m_source->hasChunksAt(pos, 1))
				continue;

			for (; pos.y <= m_max.y; pos.y++)
			{
				Brightness_t currentBrightness = m_source->getBrightness(m_lightLayer, pos);

				TileID currentTile = m_source->getTile(pos);
				Brightness_t lightBlockLevel = std::min(Tile::lightBlock[currentTile], static_cast<Brightness_t>(1));

				Brightness_t maxBrightness = Brightness::MIN;
				if (&m_lightLayer == &LightLayer::Sky)
				{
					if (m_source->canSeeSky(pos))
						maxBrightness = Brightness::MAX;
				}
				else if (&m_lightLayer == &LightLayer::Block)
				{
					maxBrightness = Tile::lightEmission[currentTile];
				}

				Brightness_t newBrightness;
				if (lightBlockLevel < Brightness::MAX || maxBrightness != Brightness::MIN)
				{
					Brightness_t brightestNeighbor = 0;
					for (int i = 0; i < Facing::COUNT; i++)
					{
						Brightness_t neighborBrightness = m_source->getBrightness(m_lightLayer, pos + Facing::DIRECTION[i]);
						if (neighborBrightness > brightestNeighbor)
							brightestNeighbor = neighborBrightness;
					}

					Brightness_t b = std::min(static_cast<Brightness_t>(brightestNeighbor - lightBlockLevel), Brightness::MAX);
					newBrightness = std::min(b, maxBrightness);
				}
				else
				{
					newBrightness = Brightness::MIN;
				}

				if (currentBrightness != newBrightness)
				{
					m_source->setBrightness(m_lightLayer, pos, newBrightness);

					Brightness_t spreadBrightness = std::max(static_cast<Brightness_t>(newBrightness - 1), Brightness::MIN);

					// why dont we check if we're in range here?
					m_source->updateLightIfOtherThan(m_lightLayer, pos - TilePos(1, 0, 0), spreadBrightness);
					m_source->updateLightIfOtherThan(m_lightLayer, pos - TilePos(0, 1, 0), spreadBrightness);
					m_source->updateLightIfOtherThan(m_lightLayer, pos - TilePos(0, 0, 1), spreadBrightness);

					if ((pos.x + 1) >= m_max.x)
						m_source->updateLightIfOtherThan(m_lightLayer, pos + TilePos(1, 0, 0), spreadBrightness);
					if ((pos.y + 1) >= m_max.y)
						m_source->updateLightIfOtherThan(m_lightLayer, pos + TilePos(0, 1, 0), spreadBrightness);
					if ((pos.z + 1) >= m_max.z)
						m_source->updateLightIfOtherThan(m_lightLayer, pos + TilePos(0, 0, 1), spreadBrightness);
				}
			}
		}
	}
}

void LightUpdate::updateFast()
{
	if (!m_source)
		return;

	// TODO!!! currently use normal update
	update();
}

bool LightUpdate::expandIfCloseEnough(const TilePos& lowerPos, const TilePos& upperPos)
{
	if (lowerPos >= m_min && upperPos <= m_max)
		return true;

	if (lowerPos < (m_min - 1) || upperPos > (m_max + 1))
		return false;

	TilePos newMin = m_min.min(lowerPos);
	TilePos newMax = m_max.max(upperPos);

	if ((newMax - newMin).volume() > 2)
		return false;

	m_min = newMin;
	m_max = newMax;
	return true;
}

void LightUpdate::expandToContain(const TilePos& pos)
{
	m_min = m_min.min(pos);
	m_max = m_max.max(pos);
}

void LightUpdate::expandToContain(const TilePos& pos1, const TilePos& pos2)
{
	expandToContain(pos1);
	expandToContain(pos2);
}
