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

	virtual void addAABBs(const Level*, const TilePos& pos, const AABB*, std::vector<AABB>&) override;
	virtual bool isSolidRender() const override;
	virtual bool isCubeShaped() const override;
	virtual int getRenderShape() const override;

	// Just overloads to forward to parent tile.
	virtual void addLights(Level*, const TilePos& pos) override;
	virtual void animateTick(Level*, const TilePos& pos, Random*) override;
	virtual void updateShape(const LevelSource*, const TilePos& pos) override;
	virtual float getBrightness(const LevelSource*, const TilePos& pos) const override;
	virtual int getTexture(Facing::Name face) const override;
	virtual int getTexture(Facing::Name face, TileData data) const override;
	virtual int getTexture(const LevelSource*, const TilePos& pos, Facing::Name face) const override;
	virtual AABB getTileAABB(const Level*, const TilePos& pos) override;
	virtual bool mayPick() const override;
	virtual bool mayPick(TileData data, bool) const override;
	virtual bool mayPlace(const Level*, const TilePos& pos) const override;
	virtual int getTickDelay() const override;
	virtual void tick(Level*, const TilePos& pos, Random*) override;
	virtual void destroy(Level*, const TilePos& pos, TileData data) override;
	virtual void onPlace(Level*, const TilePos& pos) override;
	virtual void onRemove(Level*, const TilePos& pos) override;
	virtual int getResource(TileData, Random*) const override;
	virtual int getResourceCount(Random*) const override;
	virtual void spawnResources(Level*, const TilePos& pos, TileData data) override;
	virtual void spawnResources(Level*, const TilePos& pos, TileData data, float) override;
	virtual float getExplosionResistance(Entity*) const override;
	virtual void wasExploded(Level*, const TilePos& pos) override;
	virtual int getRenderLayer() const override;
	virtual int use(Level*, const TilePos& pos, Player*) override;
	virtual void stepOn(Level*, const TilePos& pos, Entity*) override;
	virtual void setPlacedBy(Level*, const TilePos& pos, Mob*) override;
	virtual void prepareRender(Level*, const TilePos& pos) override;
	virtual void attack(Level*, const TilePos& pos, Player*) override;
	virtual void handleEntityInside(Level*, const TilePos& pos, const Entity*, Vec3&) override;

public:
	Tile* m_pParent;
};
