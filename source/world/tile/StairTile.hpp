/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Tile.hpp"

class StairTile : public Tile
{
public:
	StairTile(int ID, Tile* pParent);

	void addAABBs(const Level*, const TilePos& pos, const AABB*, std::vector<AABB>&) override;
	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	int getRenderShape() const override;

	// Just overloads to forward to parent tile.
	void addLights(Level*, const TilePos& pos) override;
	void animateTick(Level*, const TilePos& pos, Random*) override;
	void updateShape(const LevelSource*, const TilePos& pos) override;
	float getBrightness(const LevelSource*, const TilePos& pos) const override;
	int getTexture(Facing::Name face) const override;
	int getTexture(Facing::Name face, TileData data) const override;
	int getTexture(const LevelSource*, const TilePos& pos, Facing::Name face) const override;
	AABB getTileAABB(const Level*, const TilePos& pos) override;
	bool mayPick() const override;
	bool mayPick(TileData data, bool) const override;
	bool mayPlace(const Level*, const TilePos& pos) const override;
	int getTickDelay() const override;
	void tick(Level*, const TilePos& pos, Random*) override;
	void destroy(Level*, const TilePos& pos, TileData data) override;
	void onPlace(Level*, const TilePos& pos) override;
	void onRemove(Level*, const TilePos& pos) override;
	int getResource(TileData, Random*) const override;
	int getResourceCount(Random*) const override;
	void spawnResources(Level*, const TilePos& pos, TileData data) override;
	void spawnResources(Level*, const TilePos& pos, TileData data, float) override;
	float getExplosionResistance(Entity*) const override;
	void wasExploded(Level*, const TilePos& pos) override;
	int getRenderLayer() const override;
	bool use(Level*, const TilePos& pos, Player*) override;
	void stepOn(Level*, const TilePos& pos, Entity*) override;
	void setPlacedBy(Level*, const TilePos& pos, Mob*) override;
	void prepareRender(Level*, const TilePos& pos) override;
	void attack(Level*, const TilePos& pos, Player*) override;
	void handleEntityInside(Level*, const TilePos& pos, const Entity*, Vec3&) override;

public:
	Tile* m_pParent;
};
