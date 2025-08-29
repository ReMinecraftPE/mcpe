#include <assert.h>
#include <cstring>

#include "DataIO.hpp"

#define STR16_MAX_LEN 32767
#define STR32_MAX_LEN 2147483647

std::string IDataInput::_readString()
{
	int16_t size = readInt16();

	if (size <= 0)
	{
		return "\0\0\0\0";
	}

	int16_t csize, cend;
	if (size == STR16_MAX_LEN)
    {
		csize = STR16_MAX_LEN;
		cend = STR16_MAX_LEN - 1;
		size = cend;
    }
    else
    {
		csize = size + 1;
		cend = size;
    }

    char* cstr = new char[csize];
    readBytes(cstr, size);

    cstr[cend] = '\0';

    std::string str = std::string(cstr);

    if (cstr)
      delete[] cstr;

    return str;
}

std::string IDataInput::_readLongString()
{
	// @NOTE: ugly copy-paste of _readString(), just with 32-bit ints instead of 16-bit

	int32_t size = readInt32();

	if (size <= 0)
	{
		return "\0\0\0\0";
	}

	int32_t csize, cend;
	if (size == STR32_MAX_LEN)
    {
		csize = STR32_MAX_LEN;
		cend = STR32_MAX_LEN - 1;
		size = cend;
    }
    else
    {
		csize = size + 1;
		cend = size;
    }

    char* cstr = new char[csize];
    readBytes(cstr, size);

    cstr[cend] = '\0';

    std::string str = std::string(cstr);

    if (cstr)
      delete[] cstr;

    return str;
}

inline void IDataOutput::_writeString(const std::string& v)
{
	// Writing strings longer than this wont work. Hopefully we're not relying on it.
	assert(v.length() <= STR16_MAX_LEN);

	int16_t len = v.length() & STR16_MAX_LEN;
	writeInt16(len);
	writeBytes((const void *)v.c_str(), len);
}

inline void IDataOutput::_writeLongString(const std::string& v)
{
	// originally no assertion existed here, I guess Mojang assumed no string could possibly be larger than the 32-bit signed integer limit
	assert(v.length() <= STR32_MAX_LEN);

	int32_t len = v.length();
	writeInt32(len);
	writeBytes((const void *)v.c_str(), len);
}

std::string BytesDataInput::readString()
{
	return _readString();
}

std::string BytesDataInput::readLongString()
{
	return _readLongString();
}

float BytesDataInput::readFloat()
{
	float v = 0.0f;
	readBytes(&v, 4);
	return v;
}

double BytesDataInput::readDouble()
{
	double v = 0.0;
	readBytes(&v, 8);
	return v;
}

int8_t BytesDataInput::readInt8()
{
	int8_t v = 0;
	readBytes(&v, 1);
	return v;
}

int16_t BytesDataInput::readInt16()
{
	int16_t v = 0;
	readBytes(&v, 2);
	return v;
}

int32_t BytesDataInput::readInt32()
{
	int32_t v = 0;
	readBytes(&v, 4);
	return v;
}

int64_t BytesDataInput::readInt64()
{
	int64_t v = 0;
	readBytes(&v, 8);
	return v;
}

void BytesDataOutput::writeString(const std::string& v)
{
	_writeString(v);
}

void BytesDataOutput::writeLongString(const std::string& v)
{
	_writeLongString(v);
}

void BytesDataOutput::writeFloat(float v)
{
	writeBytes(&v, 4);
}

void BytesDataOutput::writeDouble(double v)
{
	writeBytes(&v, 8);
}

void BytesDataOutput::writeInt8(int8_t v)
{
	writeBytes(&v, 1);
}

void BytesDataOutput::writeInt16(int16_t v)
{
	writeBytes(&v, 2);
}

void BytesDataOutput::writeInt32(int32_t v)
{
	writeBytes(&v, 4);
}

void BytesDataOutput::writeInt64(int64_t v)
{
	writeBytes(&v, 8);
}

unsigned int StringByteInput::numBytesLeft() const
{
	return m_buffer.length() - m_idx;
}

bool StringByteInput::readBytes(void* data, unsigned int count)
{
	if (m_idx == m_buffer.length())
		return false;
	unsigned int v7 = numBytesLeft();
	if (count < v7)
		v7 = count;
	memcpy(data, &m_buffer.c_str()[m_idx], v7);
	m_idx += v7;
	return true;
}

void StringByteOutput::writeBytes(const void* data, unsigned int count)
{
	m_buffer->append((const char*)data, count);
}

float BigEndianStringByteInput::readFloat()
{
	float v = 0.0f;
	readBigEndianBytes(&v, 4);
	return v;
}

double BigEndianStringByteInput::readDouble()
{
	double v = 0.0;
	readBigEndianBytes(&v, 8);
	return v;
}

int16_t BigEndianStringByteInput::readInt16()
{
	int16_t v = 0;
	readBigEndianBytes(&v, 2);
	return v;
}

int32_t BigEndianStringByteInput::readInt32()
{
	int32_t v = 0;
	readBigEndianBytes(&v, 4);
	return v;
}

int64_t BigEndianStringByteInput::readInt64()
{
	int64_t v = 0;
	readBigEndianBytes(&v, 8);
	return v;
}

bool BigEndianStringByteInput::readBigEndianBytes(void* data, unsigned int count)
{
	if (!readBytes(data, count))
		return false;

	int8_t* buffer = (int8_t*)data;

	int8_t* v7 = &buffer[count - 1];
	if (!buffer || !v7)
		return false;

	int8_t v8;
	for (; buffer <= v7; v7--)
	{
		v8 = *buffer;
		*buffer++ = *v7;
		*v7 = v8;
	}

	return true;
}