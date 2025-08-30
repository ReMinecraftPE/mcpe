#pragma once

#include <string>
#include <stdint.h>

#include "compat/LegacyCPP.hpp"

class IDataInput
{
protected:
	std::string _readString();
	std::string _readLongString();

public:
	bool hasBytesLeft(unsigned int bytes) const { return numBytesLeft() >= bytes; }

public:
	virtual std::string readString() = 0;
	virtual std::string readLongString() = 0;
	virtual float readFloat() = 0;
	virtual double readDouble() = 0;
	virtual int8_t readInt8() = 0;
	virtual int16_t readInt16() = 0;
	virtual int32_t readInt32() = 0;
	virtual int64_t readInt64() = 0;
	virtual bool readBytes(void* data, unsigned int count) = 0;
	virtual unsigned int numBytesLeft() const = 0;
	//virtual bool isOk() const = 0;
	//virtual bool seek(uint64_t pos) = 0;
};

class IDataOutput
{
protected:
	void _writeString(const std::string& v);
	void _writeLongString(const std::string& v);

public:
	virtual void writeString(const std::string& v) = 0;
	virtual void writeLongString(const std::string& v) = 0;
	virtual void writeFloat(float v) = 0;
	virtual void writeDouble(double v) = 0;
	virtual void writeInt8(int8_t v) = 0;
	virtual void writeInt16(int16_t v) = 0;
	virtual void writeInt32(int32_t v) = 0;
	virtual void writeInt64(int64_t v) = 0;
	virtual void writeBytes(const void* data, unsigned int count) = 0;
	//virtual bool isOk() = 0;
};

class BytesDataInput : public IDataInput
{
public:
	std::string readString() override;
	std::string readLongString() override;
	float readFloat() override;
	double readDouble() override;
	int8_t readInt8() override;
	int16_t readInt16() override;
	int32_t readInt32() override;
	int64_t readInt64() override;
};

class BytesDataOutput : public IDataOutput
{
public:
	void writeString(const std::string& v) override;
	void writeLongString(const std::string& v) override;
	void writeFloat(float v) override;
	void writeDouble(double v) override;
	void writeInt8(int8_t v) override;
	void writeInt16(int16_t v) override;
	void writeInt32(int32_t v) override;
	void writeInt64(int64_t v) override;
};

class StringByteInput : public BytesDataInput
{
public:
	StringByteInput(const std::string& buffer)
		: m_idx(0), m_buffer(buffer)
	{ };

public:
	unsigned int numBytesLeft() const override;
	bool readBytes(void* data, unsigned int count) override;

public:
	uint64_t m_idx;
	std::string m_buffer;
};

class StringByteOutput : public BytesDataOutput
{
public:
	StringByteOutput(std::string* buffer)
		: m_buffer(buffer)
	{ };

public:
	void writeBytes(const void* data, unsigned int count) override;

public:
	std::string* m_buffer;
};

class BigEndianStringByteInput : public StringByteInput
{
public:
	BigEndianStringByteInput(const std::string& buffer) : StringByteInput(buffer) {}

public:
	float readFloat() override;
	double readDouble() override;
	int16_t readInt16() override;
	int32_t readInt32() override;
	int64_t readInt64() override;
	virtual bool readBigEndianBytes(void* data, unsigned int count);
};