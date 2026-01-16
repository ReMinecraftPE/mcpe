#pragma once

#include "Tile.hpp"

class BedTile : public Tile
{
public:
	BedTile(TileID id, int texture);

	int getTexture(Facing::Name face, TileData data) const override;
	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	eRenderShape getRenderShape() const override;
	void updateShape(const LevelSource* level, const TilePos& pos) override;
	void updateDefaultShape() override;
	void neighborChanged(Level* level, const TilePos& pos, TileID tile) override;
	bool use(Level*, const TilePos& pos, Player*) override;
	int getResource(TileData data, Random* random) const override;
	void spawnResources(Level*, const TilePos& pos, TileData, float) override;

	static const int headBlockToFootBlockMap[4][2];
	static const Facing::Name hiddenFace[4];
	static const int hiddenFaceIndex[4];
	static const int bedDirection[4][6];

	// Returns the direction index (0-3) for array lookups
	static int getDirectionIndex(TileData meta) {
		return meta & 3;
	}

	// Returns the semantic direction as Facing::Name enum
	static Facing::Name getDirectionFromData(TileData meta) {
		switch (meta & 3) {
			case 0: return Facing::SOUTH;
			case 1: return Facing::WEST;
			case 2: return Facing::NORTH;
			case 3: return Facing::EAST;
			default: return Facing::SOUTH;
		}
	}

	static bool isHead(TileData meta) {
		return (meta & 8) != 0;
	}

	static bool isBedOccupied(TileData meta) {
		return (meta & 4) != 0;
	}

	static void setBedOccupied(Level* level, const TilePos& pos, bool occupied);
	static TilePos getRespawnTilePos(const Level* level, const TilePos& pos, int steps);
};
