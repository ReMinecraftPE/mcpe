#include "EmptyLevelChunk.hpp"
#include "world/tile/Tile.hpp"

EmptyLevelChunk::EmptyLevelChunk(Level* pLevel, TileID* pBlockData, const ChunkPos& pos)
	:LevelChunk(pLevel, pBlockData, pos)
{
}

int EmptyLevelChunk::getHeightmap(const ChunkTilePos& pos)
{
	return 0;
}

void EmptyLevelChunk::recalcHeightmap()
{

}

void EmptyLevelChunk::recalcHeightmapOnly()
{

}

int EmptyLevelChunk::getRawBrightness(const ChunkTilePos& pos, int skySubtract)
{
	return 7;
}

void EmptyLevelChunk::addEntity(Entity*)
{

}

void EmptyLevelChunk::removeEntity(Entity*)
{

}

void EmptyLevelChunk::removeEntity(Entity*, int vec)
{

}

bool EmptyLevelChunk::isSkyLit(const ChunkTilePos& pos)
{
	return false;
}

void EmptyLevelChunk::lightLava()
{

}

void EmptyLevelChunk::recalcBlockLights()
{

}

void EmptyLevelChunk::skyBrightnessChanged()
{

}

void EmptyLevelChunk::load()
{

}

void EmptyLevelChunk::unload()
{

}

void EmptyLevelChunk::markUnsaved()
{

}

TileID EmptyLevelChunk::getTile(const ChunkTilePos& pos)
{
	return Tile::invisible_bedrock->m_ID;
}

bool EmptyLevelChunk::setTile(const ChunkTilePos& pos, TileID tile)
{
	return true;
}

bool EmptyLevelChunk::setTileAndData(const ChunkTilePos& pos, TileID tile, int data)
{
	return true;
}

int EmptyLevelChunk::getData(const ChunkTilePos& pos)
{
	return 0;
}

void EmptyLevelChunk::setData(const ChunkTilePos& pos, int data)
{

}

void EmptyLevelChunk::recalcHeight(const ChunkTilePos& pos)
{

}

bool EmptyLevelChunk::isEmpty()
{
	return true;
}
