#pragma once

#include "Utils.hpp"
#include "ChunkSource.hpp"
class Level;

class TestChunkSource : public ChunkSource
{
public:
	TestChunkSource(Level*);
	virtual ~TestChunkSource();

	LevelChunk* generateChunk(int x, int z);
	LevelChunk* create(int x, int z) override;
	LevelChunk* getChunk(int x, int z) override;
	bool hasChunk(int x, int z) override;
	std::string gatherStats() override;
	void postProcess(ChunkSource*, int, int) override;
	bool shouldSave() override;
	int tick() override;

public:
	Level* m_pLevel = nullptr;

	LevelChunk* m_chunkMap[C_MAX_CHUNKS_Z][C_MAX_CHUNKS_X];
	LevelChunk* m_pEmptyChunk;
	LevelChunk* m_pLastChunk;
	int m_lastChunkX, m_lastChunkZ;
};
