#pragma once

#include <string>
#include <vector>

class Level;
class LevelChunk;

class ChunkStorage
{
public:
	virtual ~ChunkStorage();
	virtual LevelChunk* load(Level*, int, int);
	virtual void save(Level*, LevelChunk*);
	virtual void saveEntities(Level*, LevelChunk*);
	virtual void saveAll(Level*, std::vector<LevelChunk*>&);
	virtual void tick();
	virtual void flush();
};

