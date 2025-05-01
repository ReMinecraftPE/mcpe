#pragma once

#include <cstdio>
#include <string>
#include <cassert>
#include <map>
#include "BitStream.h"
#include "world/level/levelgen/chunk/ChunkPos.hpp"

class RegionFile
{
public:
	RegionFile(const std::string fileName);
	~RegionFile();
	void close();
	bool open();
	bool readChunk(const ChunkPos& pos, RakNet::BitStream**);
	bool write(int index, RakNet::BitStream&);
	bool writeChunk(const ChunkPos& pos, RakNet::BitStream&);

public:
	FILE* m_pFile;
	std::string m_fileName;
	int* field_20;
	int* field_24;
	std::map<int, bool> field_28;
};

