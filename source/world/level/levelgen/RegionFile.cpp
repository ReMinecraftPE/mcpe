#include "RegionFile.hpp"

#define SECTOR_BYTES (4096)

static void void_sub(int a, int b)
{
}

#define WRITE(data, elemsize, elemcnt, file) void_sub(int(fwrite(data, elemsize, elemcnt, file)), elemcnt)
#define READ( data, elemsize, elemcnt, file) void_sub(int(fread (data, elemsize, elemcnt, file)), elemcnt)

RegionFile::RegionFile(const std::string fileName)
{
	m_fileName = fileName + "/" + "chunks.dat";

	field_20 = new int[1024];
	field_24 = new int[1024];
	memset(field_24, 0, 1024 * sizeof(int));
}

RegionFile::~RegionFile()
{
	close();
	if (field_20) delete[] field_20;
	if (field_24) delete[] field_24;
}

void RegionFile::close()
{
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile = nullptr;
	}
}

bool RegionFile::open()
{
	close();
	memset(field_20, 0, 1024 * sizeof(int));
	
	m_pFile = fopen(m_fileName.c_str(), "r+b");
	if (m_pFile)
	{
		READ(field_20, sizeof(int), 1024, m_pFile);
		
		field_28[0] = false;

		for (int i = 0; i < 1024; i++)
		{
			int v13 = this->field_20[i];
			if (v13)
			{
				int v12 = v13 >> 8;
				int v11 = uint8_t(v13);
				for (int j = 0; j < v11; ++j)
				{
					field_28[j + v12] = false;
				}
			}
		}

		return m_pFile != nullptr;
	}

	m_pFile = fopen(m_fileName.c_str(), "w+b");
	if (!m_pFile)
		return false;

	WRITE(field_20, sizeof(int), 1024, m_pFile);
	field_28[0] = false;

	return true;
}

bool RegionFile::readChunk(int x, int z, RakNet::BitStream** pBitStream)
{
	int idx = field_20[32 * z + x];
	if (!idx)
		return false;

	int thing = (idx >> 8);
	int offset = (idx & 0xFF);

	int length = 0;
	fseek(m_pFile, thing * SECTOR_BYTES, SEEK_SET);
	fread(&length, sizeof(int), 1, m_pFile);
	
	assert(length < ((offset & 0xff) * SECTOR_BYTES));

	length -= 4;

	uint8_t* data = new uint8_t[length];
	READ(data, 1, length, m_pFile);

	*pBitStream = new RakNet::BitStream(data, length, false);
	return true;
}

bool RegionFile::write(int index, RakNet::BitStream& bitStream)
{
	fseek(m_pFile, index * SECTOR_BYTES, 0);
	int length = sizeof(int) + bitStream.GetNumberOfBytesUsed();

	WRITE(&length, sizeof(length), 1, m_pFile);
	WRITE(bitStream.GetData(), 1, bitStream.GetNumberOfBytesUsed(), m_pFile);

	return true;
}

bool RegionFile::writeChunk(int x, int z, RakNet::BitStream& bitStream)
{
	int length = bitStream.GetNumberOfBytesUsed();
	int field20i = field_20[32 * z + x];
	int lowerIndex = (length + 4) / SECTOR_BYTES + 1;
	if (lowerIndex > 256)
		return false;

	int field20iU = field20i >> 8, field20iL = field20i & 0xff;

	if (field20iU && lowerIndex == field20iL)
	{
		write(field20iU, bitStream);
		return true;
	}
	
	for (int i = 0; i < field20iL; i++)
	{
		field_28[i + field20iU] = true;
	}

	bool bNeedWrite = false;
	int v22 = 0, i = 0;
	while (i < lowerIndex)
	{
		if (field_28.find(i + v22) == field_28.end())
		{
			bNeedWrite = true;
			break;
		}

		if (field_28[i + v22])
		{
			i++;
		}
		else
		{
			v22 += i + 1;
			i = 0;
		}
	}

	if (bNeedWrite)
	{
		fseek(m_pFile, 0, SEEK_END);
		for (int j = 0; lowerIndex - i > j; j++)
		{
			fwrite(field_24, sizeof(int), 1024, m_pFile);
			field_28[j + v22] = true;
		}
	}

	field_20[32 * z + x] = (v22 << 8) | lowerIndex;
	for (int k = 0; k < lowerIndex; k++)
	{
		field_28[k + v22] = false;
	}

	write(v22, bitStream);
	fseek(m_pFile, sizeof(int) * (x + 32 * z), SEEK_SET);
	fwrite(&field_20[x + 32 * z], sizeof(int), 1, m_pFile);

	return true;
}
