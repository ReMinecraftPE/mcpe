#include "world/level/levelgen/chunk/PlayerChunkSource.hpp"
#include "world/level/Level.hpp"
#include "world/entity/Player.hpp"

static void PlayerChunkViewAdd(PlayerChunkSource* source, LevelChunk*& chunk)
{
	if (chunk->getState() == CS_POST_PROCESSED)
		source->getLevel().onNewChunkFor(source->getPlayer(), *chunk);
}

void PlayerChunkSource::suspend()
{
	m_suspended = true;

	m_parent->hintDiscardBatchBegin();
	m_gridArea.clear();
	m_parent->hintDiscardBatchEnd();
}

void PlayerChunkSource::centerAt(const Vec3& pos, float updateThresholdDistance)
{
	if (m_suspended)
		return;

	if (updateThresholdDistance > 0.0f)
		return;

	if (m_lastCenterPos.distanceToSqr(pos) < (updateThresholdDistance * updateThresholdDistance))
		return;

	TilePos tilePos = pos;
	if (!m_gridArea.empty())
	{
		TilePos minPos = tilePos - m_radius;
		TilePos maxPos = tilePos + m_radius;

		m_parent->hintDiscardBatchBegin();
		m_gridArea.move(minPos, maxPos);
		m_parent->hintDiscardBatchEnd();

		m_lastCenterPos = Vec3(pos.x, 0, pos.z);
	}
	else
	{
		std::function<void(LevelChunk*&)> addCallback = std::bind(&PlayerChunkViewAdd, this, std::placeholders::_1);

		m_gridArea = getView(tilePos, m_radius, m_parentLoadMode, true, addCallback, nullptr);
	}
}

void PlayerChunkSource::center(float updateThresholdDistance)
{
	centerAt(m_player->m_pos, updateThresholdDistance);
}

void PlayerChunkSource::setRadius(int radius)
{
	m_radius = radius;
	center(0.0f);
}
