#pragma once
#include "world/level/levelgen/chunk/ChunkViewSource.hpp"

class Player;

class PlayerChunkSource : public ChunkViewSource
{
protected:
	Player* m_player;
	int m_radius;
	bool m_suspended;
	Vec3 m_lastCenterPos;

public:
	PlayerChunkSource(Player& player, ChunkSource& parentSource, int radius);
	virtual ~PlayerChunkSource();

public:
	void suspend();
	void centerAt(const Vec3& pos, float updateThresholdDistance = 16.0f);
	void center(float updateThresholdDistance = 16.0f);
	void setRadius(int radius);

	Player& getPlayer()
	{
		return *m_player;
	}
};
