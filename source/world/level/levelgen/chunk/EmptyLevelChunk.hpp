#pragma once

#include "LevelChunk.hpp"

//@OVERSIGHT: Why the hell is EmptyLevelChunk derived from the WHOLE of LevelChunk?!
class EmptyLevelChunk : public LevelChunk
{
public:
	EmptyLevelChunk(Level*, TileID* pBlockData, const ChunkPos& pos);

	int getHeightmap(const ChunkTilePos& pos) override;
	void recalcHeightmap() override;
	void recalcHeightmapOnly() override;
	int getRawBrightness(const ChunkTilePos& pos, int skySubtract) override;
	void addEntity(Entity*) override;
	void removeEntity(Entity*) override;
	void removeEntity(Entity*, int vec) override;
	bool isSkyLit(const ChunkTilePos& pos) override;
	void lightLava() override;
	void recalcBlockLights() override;
	void skyBrightnessChanged() override;
	void load() override;
	void unload() override;
	void markUnsaved() override;
	TileID getTile(const ChunkTilePos& pos) override;
	bool setTile(const ChunkTilePos& pos, TileID tile) override;
	bool setTileAndData(const ChunkTilePos& pos, TileID tile, TileData data) override;
	TileData getData(const ChunkTilePos& pos) override;
	bool setData(const ChunkTilePos& pos, TileData data) override;
	void recalcHeight(const ChunkTilePos& pos) override;
	bool isEmpty() override;
};