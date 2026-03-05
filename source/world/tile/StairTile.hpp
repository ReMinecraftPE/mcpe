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

public:
	void addAABBs(TileSource*, const TilePos& pos, const AABB*, std::vector<AABB>&) override;
	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	eRenderShape getRenderShape() const override;

	// Just overloads to forward to parent tile.
	void addLights(TileSource*, const TilePos& pos) override;
	void animateTick(TileSource*, const TilePos& pos, Random*) override;
	void updateShape(TileSource*, const TilePos& pos) override;
	float getBrightness(TileSource*, const TilePos& pos) const override;
	int getTexture(Facing::Name face) const override;
	int getTexture(Facing::Name face, TileData data) const override;
	int getTexture(TileSource*, const TilePos& pos, Facing::Name face) const override;
	AABB getTileAABB(TileSource*, const TilePos& pos) override;
	bool mayPick() const override;
	bool mayPick(TileData data, bool) const override;
	bool mayPlace(TileSource*, const TilePos& pos) const override;
	int getTickDelay() const override;
	void tick(TileSource*, const TilePos& pos, Random*) override;
	void destroy(TileSource*, const TilePos& pos, TileData data) override;
	void onPlace(TileSource*, const TilePos& pos) override;
	void onRemove(TileSource*, const TilePos& pos) override;
	float getExplosionResistance(Entity*) const override;
	void wasExploded(TileSource*, const TilePos& pos) override;
	RenderLayer getRenderLayer(TileSource*, const TilePos&) const override;
	bool use(const TilePos& pos, Player*) override;
	void stepOn(TileSource*, const TilePos& pos, Entity*) override;
	void setPlacedBy(TileSource*, const TilePos& pos, Mob*) override;
	void prepareRender(TileSource*, const TilePos& pos) override;
	void attack(TileSource*, const TilePos& pos, Player*) override;
	void handleEntityInside(TileSource*, const TilePos& pos, const Entity*, Vec3&) override;

public:
	Tile* m_pParent;
};
