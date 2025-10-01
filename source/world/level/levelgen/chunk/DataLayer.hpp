#pragma once

#include <stdint.h>

#include "world/level/levelgen/chunk/ChunkTilePos.hpp"

class DataLayer
{
public:
	DataLayer();
	DataLayer(unsigned int size);

public:
	uint8_t get(const ChunkTilePos& pos);
	void set(const ChunkTilePos& pos, uint8_t data);

public:
	uint8_t* m_data;
	unsigned int m_size;
};