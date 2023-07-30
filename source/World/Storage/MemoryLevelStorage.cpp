#include "MemoryLevelStorage.hpp"
#include "MemoryChunkStorage.hpp"
#include "LevelData.hpp"

LevelData* MemoryLevelStorage::prepareLevel(Level* pLevel)
{
	return nullptr;
}

ChunkStorage* MemoryLevelStorage::createChunkStorage(Dimension* pDim)
{
	return new MemoryChunkStorage;
}

void MemoryLevelStorage::saveLevelData(LevelData* levelData, std::vector<Player*>& players)
{
}

void MemoryLevelStorage::saveLevelData(LevelData* levelData)
{
}

void MemoryLevelStorage::closeAll()
{
}
