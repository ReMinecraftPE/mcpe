#include "RakIO.hpp"

unsigned int RakDataInput::numBytesLeft() const
{
	return (m_bitStream.GetWriteOffset() - m_bitStream.GetReadOffset()) / 8;
}

bool RakDataInput::readBytes(void* data, unsigned int count)
{
	if (!hasBytesLeft(count))
		return false;

	RakNet::BitSize_t bitsToRead = 8 * count;
	if (bitsToRead > 0)
		m_bitStream.ReadBits((unsigned char*)data, bitsToRead);

	return true;
}

void RakDataOutput::writeBytes(const void* data, unsigned int count)
{
	m_bitStream.WriteBits((const unsigned char*)data, 8 * count);
}