#include "RegionFile.hpp"

#define SECTOR_BYTES (4096)

static void void_sub(int a, int b)
{
}

#define WRITE(data, elemsize, elemcnt, file) void_sub(int(fwrite(data, elemsize, elemcnt, file)), elemcnt)
#define READ( data, elemsize, elemcnt, file) void_sub(int(fread (data, elemsize, elemcnt, file)), elemcnt)

RegionFile::RegionFile(const std::string fileName)
{
	m_pFile = nullptr;
	m_fileName = fileName + "/" + "chunks.dat";

	m_offsets = new int[1024];
	m_emptyChunk = new int[1024];
	memset(m_emptyChunk, 0, 1024 * sizeof(int));
}

RegionFile::~RegionFile()
{
	close();
	if (m_offsets) delete[] m_offsets;
	if (m_emptyChunk) delete[] m_emptyChunk;
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
	memset(m_offsets, 0, 1024 * sizeof(int));
	
	m_pFile = fopen(m_fileName.c_str(), "r+b");
	if (m_pFile)
	{
		READ(m_offsets, sizeof(int), 1024, m_pFile);
		
		m_sectorFree[0] = false;

		for (int i = 0; i < 1024; i++)
		{
			int v13 = this->m_offsets[i];
			if (v13)
			{
				int v12 = v13 >> 8;
				int v11 = uint8_t(v13);
				for (int j = 0; j < v11; ++j)
				{
					m_sectorFree[j + v12] = false;
				}
			}
		}

		return m_pFile != nullptr;
	}

	m_pFile = fopen(m_fileName.c_str(), "w+b");
	if (!m_pFile)
		return false;

	WRITE(m_offsets, sizeof(int), 1024, m_pFile);
	m_sectorFree[0] = false;

	return true;
}

bool RegionFile::readChunk(const ChunkPos& pos, RakNet::BitStream** pBitStream)
{
	int idx = m_offsets[32 * pos.z + pos.x];
	if (!idx)
		return false;

	int thing = (idx >> 8);

	int length = 0;
	if (fseek(m_pFile, thing * SECTOR_BYTES, SEEK_SET) != 0)
		return false;
	if (fread(&length, sizeof(int), 1, m_pFile) != 1)
		return false;
	
	assert(length < ((idx & 0xff) * SECTOR_BYTES));

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

bool RegionFile::writeChunk(const ChunkPos& pos, RakNet::BitStream& bitStream)
{
	int length = bitStream.GetNumberOfBytesUsed();
	int field20i = m_offsets[32 * pos.z + pos.x];
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
		m_sectorFree[i + field20iU] = true;
	}

	bool bNeedWrite = false;
	int v22 = 0, i = 0;
	while (i < lowerIndex)
	{
		if (m_sectorFree.find(i + v22) == m_sectorFree.end())
		{
			bNeedWrite = true;
			break;
		}

		if (m_sectorFree[i + v22])
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
		if (fseek(m_pFile, 0, SEEK_END) != 0)
			return false;
		for (int j = 0; lowerIndex - i > j; j++)
		{
			if (fwrite(m_emptyChunk, sizeof(int), 1024, m_pFile) != 1024)
				return false;
			m_sectorFree[j + v22] = true;
		}
	}

	m_offsets[32 * pos.z + pos.x] = (v22 << 8) | lowerIndex;
	for (int k = 0; k < lowerIndex; k++)
	{
		m_sectorFree[k + v22] = false;
	}

	write(v22, bitStream);
	if (fseek(m_pFile, sizeof(int) * (pos.x + 32 * pos.z), SEEK_SET) != 0)
		return false;
	if (fwrite(&m_offsets[pos.x + 32 * pos.z], sizeof(int), 1, m_pFile) != 1)
		return false;

	return true;
}
