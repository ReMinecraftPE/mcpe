#pragma once
#include <string>
class Level;
class LevelChunk;

class ChunkSource
{
public:
	virtual ~ChunkSource();
	virtual bool hasChunk(int, int) = 0;
	virtual LevelChunk* getChunk(int, int) = 0;
	virtual LevelChunk* create(int, int) = 0;
	virtual void postProcess(ChunkSource*, int, int) = 0;
	virtual int  tick() = 0;
	virtual bool shouldSave() = 0;
	virtual void saveAll();
	virtual std::string gatherStats() = 0;
};

