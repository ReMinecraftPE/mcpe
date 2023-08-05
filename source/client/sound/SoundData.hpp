/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <cstdint>

struct PCMSoundHeader
{
	int m_channels;
	int m_bytes_per_sample;
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
		field_4 = header.m_channels * header.m_length * header.m_bytes_per_sample;
	}
};
