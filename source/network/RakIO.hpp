#pragma once

#include "common/DataIO.hpp"
#include "BitStream.h"

class RakDataInput : public BytesDataInput
{
public:
	RakDataInput(RakNet::BitStream& bitStream)
		: m_bitStream(bitStream)
	{ };
	
public:
	virtual unsigned int numBytesLeft() const override;
	virtual bool readBytes(void* data, unsigned int count) override;

public:
	RakNet::BitStream& m_bitStream;
};

class RakDataOutput : public BytesDataOutput
{
public:
	RakDataOutput(RakNet::BitStream& bitStream)
		: m_bitStream(bitStream)
	{ };

public:
	virtual void writeBytes(const void* data, unsigned int count) override;

public:
	RakNet::BitStream& m_bitStream;
};