#pragma once

#include "EntityTile.hpp"

class FurnaceTile : public EntityTile
{
public:
	FurnaceTile(int id, bool active);

public:
	int getTexture(const LevelSource*, const TilePos& pos, Facing::Name face) const override;
	void animateTick(Level* level, const TilePos& pos, Random* random) override;
	int getTexture(Facing::Name face) const override;
	void onPlace(Level* level, const TilePos& pos) override;
	bool use(Level* level, const TilePos& pos, Player* player) override;
	void setPlacedBy(Level*, const TilePos& pos, Mob*) override;
	void onRemove(Level* level, const TilePos& pos) override;
	bool hasTileEntity() const override;
	TileEntity* newTileEntity() override;
	int getResource(TileData, Random*) const override;

public:
	static void SetLit(bool lit, Level* level, const TilePos& pos);
	static void RecalculateLookDirection(Level* level, const TilePos& pos);

private:
	Random m_random;
	static bool keepInventory;

public:
	bool m_active;
};

