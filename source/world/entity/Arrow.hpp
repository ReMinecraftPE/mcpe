#pragma once

#include "Entity.hpp"

class Arrow : public Entity
{
private:
	TilePos m_tilePos;
	TileID m_lastTile;
	bool m_inGround;
	int m_life;
	int m_flightTime;

public:
	int m_shakeTime;
	Mob* m_owner;

	Arrow(Level* pLevel);
	Arrow(Level* pLevel, const Vec3& pos);
	Arrow(Level* pLevel, Mob* pMob);

private:
	void _init();
	void _lerpMotion(const Vec3& vel);
	void _lerpMotion2(const Vec3& vel);

public:
	void shoot(float x, float y, float z, float speed, float r) { shoot(Vec3(x, y, z), speed, r); };
	void shoot(Vec3 pos, float speed, float r);
	
	void lerpMotion(float x, float y, float z) { lerpMotion(Vec3(x, y, z)); };
	void lerpMotion(const Vec3& vel);

	void tick() override;

	void playerTouch(Player* pPlayer) override;

	float getShadowHeightOffs() const override { return 0.0f; }
};