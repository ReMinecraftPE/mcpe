/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SoundSystem_windows.hpp"

SoundSystemWindows::SoundSystemWindows()
{
	printf("Init SoundSystemWindows\n");

	WNDCLASSA WndClass;

	memset(&WndClass, 0, sizeof(WndClass));
	WndClass.lpfnWndProc = DefWindowProcA;
	WndClass.hInstance = GetModuleHandleA(NULL);
	WndClass.lpszClassName = "SoundWindow";
	RegisterClassA(&WndClass);
	m_hiddenwindow = CreateWindowExA(
		0,
		"SoundWindow",
		"SoundWindow",
		0x80000000,
		0,
		0,
		0,
		0,
		0,
		0,
		WndClass.hInstance,
		0);
	if (!m_hiddenwindow) {
		printf("SoundSystemWindows cannot create hidden window for directsound8\n");
		return;
	}

	initDirectSound();

}

void SoundSystemWindows::initDirectSound()
{
	HRESULT result;
	DSBUFFERDESC bufferDesc;


	result = DirectSoundCreate8(NULL, &m_directsound, NULL);
	if (FAILED(result))
	{
		printf("SoundSystemWindows failed to create directsound8 handle\n");
		return;
	}

	result = m_directsound->SetCooperativeLevel(m_hiddenwindow, DSSCL_NORMAL);
	if (FAILED(result))
	{
		printf("SoundSystemWindows failed set cooperation level\n");
		return;
	}

	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Get control of the primary sound buffer on the default sound device.
	result = m_directsound->CreateSoundBuffer(&bufferDesc, &m_primarybuffer, NULL);
	if (FAILED(result))
	{
		printf("SoundSystemWindows failed to create primary sound buffer\n");
		return;
	}

}


SoundSystemWindows::~SoundSystemWindows()
{
	printf("Destroying SoundSystemWindows\n");
}


bool SoundSystemWindows::isAvailable()
{
	return false;
}

void SoundSystemWindows::setListenerPos(float x, float y, float z)
{

}

void SoundSystemWindows::setListenerAngle(float f)
{

}

void SoundSystemWindows::load(const std::string& sound)
{
}

void SoundSystemWindows::play(const std::string& sound)
{
}

void SoundSystemWindows::pause(const std::string& sound)
{
}

void SoundSystemWindows::stop(const std::string& sound)
{
}


void SoundSystemWindows::playAt(const SoundDesc& sound, float x, float y, float z, float volume, float pitch)
{

	//Release sounds that finished playing
	for (size_t i = 0; i < m_buffers.size(); i++)
	{
		DWORD status;
		(*m_buffers[i])->GetStatus(&status);
		if (status != DSBSTATUS_PLAYING) {
			(*m_buffers[i])->Release();
			delete m_buffers[i];
			m_buffers.erase(m_buffers.begin() + i);
		}
	}


	if (sound.m_pHeader->m_id == 1) //for some reason if the id is 1 you need to pitch the sound down
	{
		pitch /= 2.f;
	}

	HRESULT result;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* bufferPtr;
	unsigned long bufferSize;

	int length = sound.m_pHeader->m_length*sizeof(uint16_t);

	LPDIRECTSOUNDBUFFER* soundbuffer = (LPDIRECTSOUNDBUFFER*)calloc(1, sizeof(LPDIRECTSOUNDBUFFER));

	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = DWORD(float(sound.m_pHeader->m_sample_rate) * pitch);
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = sound.m_header.m_channels;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
	DSBUFFERDESC bufferDesc;
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	if (sound.m_header.m_channels == 1) {
		bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRL3D;
	}
	else {
		bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS;
	}

	bufferDesc.dwBufferBytes = length;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Create a temporary sound buffer with the specific buffer settings.
	result = m_directsound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if (FAILED(result))
	{
		printf("SoundSystemWindows CreateSoundBuffer failed\n");
		return;
	}

	// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)soundbuffer);
	if (FAILED(result))
	{
		printf("SoundSystemWindows QueryInterface failed\n");
		return;
	}

	// Release the temporary buffer.
	tempBuffer->Release();
	tempBuffer = 0;


	// Lock the secondary buffer to write wave data into it.
	result = (*soundbuffer)->Lock(0, length, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if (FAILED(result))
	{
		printf("SoundSystemWindows lock failed\n");
		return;
		//return false;
	}

	// Copy the wave data into the buffer.
	memcpy(bufferPtr, sound.m_pData, length);

	// Unlock the secondary buffer after the data has been written to it.
	result = (*soundbuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if (FAILED(result))
	{
		printf("SoundSystemWindows unlock failed\n");
		return;
	}


	(*soundbuffer)->Play(0, 0, 0);
	m_buffers.push_back(soundbuffer);
}
