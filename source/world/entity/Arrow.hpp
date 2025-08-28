#pragma once

#include "Entity.hpp"

class Arrow : public Entity
{
public:
	static const unsigned int ARROW_BASE_DAMAGE;

private:
	void _init();

public:
	Arrow(Level* pLevel);
	Arrow(Level* pLevel, const Vec3& pos);
	Arrow(Level* pLevel, Mob* pMob);

private:
	void _lerpMotion(const Vec3& vel);
	void _lerpMotion2(const Vec3& vel);

public:
	void shoot(float x, float y, float z, float speed, float r) { shoot(Vec3(x, y, z), speed, r); };
	void shoot(Vec3 pos, float speed, float r);
	
	void lerpMotion(float x, float y, float z) { lerpMotion(Vec3(x, y, z)); };
	void lerpMotion(const Vec3& vel);

	void tick() override;
	void playerTouch(Player* pPlayer) override;
	void addAdditionalSaveData(CompoundTag& tag) const override;
	void readAdditionalSaveData(const CompoundTag& tag) override;

	float getShadowHeightOffs() const override { return 0.0f; }

private:
	TilePos m_tilePos;
	TileID m_lastTile;
	int8_t m_lastTileData;
	bool m_bInGround;
	bool m_bIsPlayerOwned;
	int m_life;
	int m_flightTime;

public:
	int m_shakeTime;
	Mob* m_owner;
};