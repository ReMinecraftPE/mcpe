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
	m_size = size >> 1;
	m_data = new uint8_t[m_size >> 1];
	memset(m_data, 0, m_size);
}

DataLayer::~DataLayer()
{
	if (m_data)
		delete[] m_data;
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
	int index1 = pos.y | (pos.x << 11) | (pos.z << 7);
	int index2 = index1 >> 1;
	uint8_t v7 = data;
	bool v8 = (index1 & 1) == 0;
	uint8_t v9 = m_data[index1 >> 1];
	uint8_t v10;
	if (v8)
	{
		v7 = data & 0xF;
		v10 = v9 & 0xF0;
	}
	else
	{
		v10 = v9 & 0xF;
	}
	uint8_t v11;
	if (v8)
		v11 = v7 | v10;
	else
		v11 = v10 | (16 * v7);
	m_data[index2] = v11;

	/*

	data &= 0xF;

	int index = pos.y | (pos.x << 11) | (pos.z << 7);

	uint8_t& xdata = m_data[index >> 1];
	if (index & 1)
		xdata = (xdata & 0x0F) | (data << 4);
	else
		xdata = (xdata & 0xF0) | (data);

	*/
}