#pragma once

#include <cstdint>

struct PCMSoundHeader
{
	int m_id;
	int m_channels;
	int m_sample_rate;
	int m_length;
};

struct SoundDesc
{
	uint16_t* m_pData;
	int field_4;
	PCMSoundHeader m_header;
	PCMSoundHeader* m_pHeader = nullptr;

	SoundDesc() {}
	SoundDesc(PCMSoundHeader& header, uint16_t* data)
	{
		m_pHeader = &header;
		m_header = header;
		m_pData = data;
		field_4 = header.m_id * header.m_length * header.m_channels;
	}
};
