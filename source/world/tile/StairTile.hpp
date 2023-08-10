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

	virtual void addAABBs(Level*, int x, int y, int z, const AABB*, std::vector<AABB>&) override;
	virtual bool isSolidRender() override;
	virtual bool isCubeShaped() override;
	virtual int getRenderShape() override;

	// Just overloads to forward to parent tile.
	virtual void addLights(Level*, int x, int y, int z) override;
	virtual void animateTick(Level*, int x, int y, int z, Random*) override;
	virtual void updateShape(LevelSource*, int, int, int) override;
	virtual float getBrightness(LevelSource*, int, int, int) override;
	virtual int getTexture(int) override;
	virtual int getTexture(int, int) override;
	virtual int getTexture(LevelSource*, int, int, int, int) override;
	virtual AABB getTileAABB(Level*, int, int, int) override;
	virtual bool mayPick() override;
	virtual bool mayPick(int, bool) override;
	virtual bool mayPlace(Level*, int, int, int) override;
	virtual int getTickDelay() override;
	virtual void tick(Level*, int, int, int, Random*) override;
	virtual void destroy(Level*, int, int, int, int dir) override;
	virtual void onPlace(Level*, int, int, int) override;
	virtual void onRemove(Level*, int, int, int) override;
	virtual int getResource(int, Random*) override;
	virtual int getResourceCount(Random*) override;
	virtual void spawnResources(Level*, int, int, int, int) override;
	virtual void spawnResources(Level*, int, int, int, int, float) override;
	virtual float getExplosionResistance(Entity*) override;
	virtual void wasExploded(Level*, int, int, int) override;
	virtual int getRenderLayer() override;
	virtual int use(Level*, int, int, int, Player*) override;
	virtual void stepOn(Level*, int, int, int, Entity*) override;
	virtual void setPlacedBy(Level*, int, int, int, Mob*) override;
	virtual void prepareRender(Level*, int, int, int) override;
	virtual void attack(Level*, int, int, int, Player*) override;
	virtual void handleEntityInside(Level*, int, int, int, Entity*, Vec3&) override;

public:
	Tile* m_pParent;
};
