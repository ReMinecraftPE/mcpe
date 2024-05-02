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

	virtual void addAABBs(const Level*, int x, int y, int z, const AABB*, std::vector<AABB>&) override;
	virtual bool isSolidRender() const override;
	virtual bool isCubeShaped() const override;
	virtual int getRenderShape() const override;

	// Just overloads to forward to parent tile.
	virtual void addLights(Level*, int x, int y, int z) override;
	virtual void animateTick(Level*, int x, int y, int z, Random*) override;
	virtual void updateShape(const LevelSource*, int, int, int) override;
	virtual float getBrightness(const LevelSource*, int, int, int) const override;
	virtual int getTexture(int) const override;
	virtual int getTexture(int, int) const override;
	virtual int getTexture(const LevelSource*, int, int, int, int) const override;
	virtual AABB getTileAABB(const Level*, int, int, int) override;
	virtual bool mayPick() const override;
	virtual bool mayPick(int, bool) const override;
	virtual bool mayPlace(const Level*, int, int, int) const override;
	virtual int getTickDelay() const override;
	virtual void tick(Level*, int, int, int, Random*) override;
	virtual void destroy(Level*, int, int, int, int dir) override;
	virtual void onPlace(Level*, int, int, int) override;
	virtual void onRemove(Level*, int, int, int) override;
	virtual int getResource(int, Random*) const override;
	virtual int getResourceCount(Random*) const override;
	virtual void spawnResources(Level*, int, int, int, int) override;
	virtual void spawnResources(Level*, int, int, int, int, float) override;
	virtual float getExplosionResistance(Entity*) const override;
	virtual void wasExploded(Level*, int, int, int) override;
	virtual int getRenderLayer() const override;
	virtual int use(Level*, int, int, int, Player*) override;
	virtual void stepOn(Level*, int, int, int, Entity*) override;
	virtual void setPlacedBy(Level*, int, int, int, Mob*) override;
	virtual void prepareRender(Level*, int, int, int) override;
	virtual void attack(Level*, int, int, int, Player*) override;
	virtual void handleEntityInside(Level*, int, int, int, const Entity*, Vec3&) override;

public:
	Tile* m_pParent;
};
