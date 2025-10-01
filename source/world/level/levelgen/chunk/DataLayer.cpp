#include <string.h>
#include <assert.h>

#include "DataLayer.hpp"

DataLayer::DataLayer()
{
	m_size = 0;
	m_data = nullptr;
}

DataLayer::DataLayer(unsigned int size)
{
	m_size = size;
	m_data = new uint8_t[m_size];
	memset(m_data, 0, m_size);
}

uint8_t DataLayer::get(const ChunkTilePos& pos)
{
	int index = pos.y | (pos.x << 11);
	uint8_t data = m_data[(index | (pos.z << 7)) >> 1];

	if ((index & 1) != 0)
		return data >> 4;
	return data & 0xF;
}

void DataLayer::set(const ChunkTilePos& pos, uint8_t data)
{
	data &= 0xF;

	int index = pos.y | (pos.x << 11) | (pos.z << 7);

	uint8_t& xdata = m_data[index >> 1];
	if (index & 1)
		xdata = (xdata & 0x0F) | (data << 4);
	else
		xdata = (xdata & 0xF0) | (data);
}