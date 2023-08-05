#pragma once

#include <cstdio>
#include <string>
#include <cassert>
#include <map>
#include "BitStream.h"

class RegionFile
{
public:
	RegionFile(const std::string fileName);
	~RegionFile();
	void close();
	bool open();
	bool readChunk(int x, int z, RakNet::BitStream**);
	bool write(int index, RakNet::BitStream&);
	bool writeChunk(int x, int z, RakNet::BitStream&);

public:
	FILE* m_pFile = nullptr;
	std::string m_fileName;
	int* field_20;
	int* field_24;
	std::map<int, bool> field_28;
};

