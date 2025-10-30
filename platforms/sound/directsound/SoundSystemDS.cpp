/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#define WIN32_LEAN_AND_MEAN
#include "CustomSoundSystem.hpp"
#include "common/Logger.hpp"
#include "client/app/AppPlatform.hpp"

// @TODO: fix crash in playAt when Asan is active

SoundSystemDS::SoundSystemDS()
{
	m_available = false;
	m_hWnd = (HWND)AppPlatform::singleton()->m_hWND;

	HRESULT result;
	DSBUFFERDESC bufferDesc;

	result = DirectSoundCreate(NULL, &m_directsound, NULL);
	if (FAILED(result))
	{
		LOG_E("SoundSystemDS failed to create DirectSound handle");
		return;
	}

	result = m_directsound->SetCooperativeLevel(m_hWnd, DSSCL_NORMAL);
	if (FAILED(result))
	{
		LOG_E("SoundSystemDS failed set cooperation level");
		return;
	}

	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Get control of the primary sound buffer on the default sound device.
	IDirectSoundBuffer* primaryBuffer;
	result = m_directsound->CreateSoundBuffer(&bufferDesc, &primaryBuffer, NULL);
	if (FAILED(result))
	{
		LOG_E("SoundSystemDS failed to create primary sound buffer");
		return;
	}

	result = primaryBuffer->QueryInterface(IID_IDirectSound3DListener,
		(LPVOID*)&m_listener);
	primaryBuffer->Release();

	if (FAILED(result))
	{
		LOG_E("SoundSystemDS failed to create 3D listener\n");
	}

	m_available = true;
}

SoundSystemDS::~SoundSystemDS()
{
	LOG_I("Destroying SoundSystemDS");

	if (!isAvailable())
	{
		return;
	}

	m_directsound->Release();
}


bool SoundSystemDS::isAvailable()
{
	return m_available;
}

void SoundSystemDS::setListenerPos(const Vec3& pos)
{
	if (!isAvailable())
	{
		return;
	}
	m_listener->SetPosition(pos.x, pos.y, -pos.z, DS3D_IMMEDIATE);
}


void SoundSystemDS::setListenerAngle(const Vec2& rot)
{
	if (!isAvailable())
	{
		return;
	}

	float yaw = rot.x * M_PI / 180.f;
	float pitch = rot.y * M_PI / 180.f;

	float lx = cosf(pitch) * sinf(yaw);
	float ly = -sinf(pitch);
	float lz = cosf(yaw);

	float ux = sinf(pitch) * sinf(yaw);
	float uy = cosf(pitch);
	float uz = sinf(pitch) * cosf(yaw);

	m_listener->SetOrientation(-lx,-ly,-lz, ux,uy,uz, DS3D_IMMEDIATE);
}

void SoundSystemDS::playAt(const SoundDesc& sound, const Vec3& pos, float volume, float pitch)
{
	if (!isAvailable())
	{
		// DirectSound failed to initialize. Return to avoid crash.
		assert(!"DirectSound is not initialized!");
		return;
	}

	// Release sounds that finished playing
	_cleanSources();

	HRESULT result;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* bufferPtr;
	unsigned long bufferSize;

	int length = sound.m_header.m_length * sound.m_header.m_bytes_per_sample;
	bool is2D = pos.length() == 0.0f;

	LPDIRECTSOUNDBUFFER soundbuffer; //= (LPDIRECTSOUNDBUFFER*)calloc(1, sizeof(LPDIRECTSOUNDBUFFER));

	WAVEFORMATEX waveFormat = _getWaveFormat(sound.m_header, pitch);

	// Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
	DSBUFFERDESC bufferDesc;
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);

	//Because directsound does not support DSBCAPS_CTRL3D on a sound with 2 channels we can only do it on sounds with 1 channel
	if (sound.m_header.m_channels == 1)
	{
		bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRL3D;
	}
	else
	{
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
		LOG_E("SoundSystemDS CreateSoundBuffer failed");
		return;
	}

	// Test the buffer format against the direct sound interface and create the secondary buffer.
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer, (LPVOID*)&soundbuffer);
	if (FAILED(result))
	{
		LOG_E("SoundSystemDS tempBuffer QueryInterface failed");
		return;
	}

	// Release the temporary buffer.
	tempBuffer->Release();
	tempBuffer = 0;


	// Lock the secondary buffer to write wave data into it.
	result = soundbuffer->Lock(0, length, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if (FAILED(result))
	{
		LOG_E("SoundSystemDS lock failed");
		return;
		//return false;
	}

	//Move the wave data into the buffer.
	memcpy(bufferPtr, sound.m_buffer.m_pData, length);

	// Unlock the secondary buffer after the data has been written to it.
	result = soundbuffer->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if (FAILED(result))
	{
		LOG_E("SoundSystemDS unlock failed");
		return;
	}

	// references:
	// https://gamedev.net/forums/topic/337397-sound-volume-question-directsound/3243306/
	// https://learn.microsoft.com/en-us/previous-versions/windows/desktop/mt708939(v=vs.85)
	// Conversion from 0-1 linear volume to directsound logarithmic volume..
	// This seems to work for the most part, but accuracy testing should be done for actual MCPE, water splashing is pretty quiet.
	// clamp the attenuation value
	float attenuation = Mth::clamp(volume, 0.0f, 1.0f);//Lerp(DSBVOLUME_MIN, DSBVOLUME_MAX, volume);

	if (attenuation == 0)
	{
		// no sound would come out, maybe skip playing this sound?
		attenuation = DSBVOLUME_MIN;
	}
	else
	{
		attenuation = floorf(2000.0f * log10f(attenuation) + 0.5f);
	}
	soundbuffer->SetVolume(LONG(attenuation));

	BufferInfo info;
	info.buffer = soundbuffer;
	info.object3d = NULL;

	//Check if position is not 0,0,0 to play 3D sound
	if (!is2D)
	{
		LPDIRECTSOUND3DBUFFER object3d;

		HRESULT hr = soundbuffer->QueryInterface(IID_IDirectSound3DBuffer,
			(LPVOID*)&object3d);
		if (FAILED(hr)) {
			LOG_E("SoundSystemDS QueryInterface failed for 3D Object");
			return;
		}

		object3d->SetPosition(pos.x, pos.y, -pos.z, DS3D_IMMEDIATE); 

		//Im not really sure what values original MCPE would use.
		object3d->SetMinDistance(2.f, DS3D_IMMEDIATE); 
		object3d->SetMaxDistance(100.f, DS3D_IMMEDIATE);

		info.object3d = object3d;
	}

	soundbuffer->Play(0, 0, 0);

	m_buffers.push_back(info);
}

WAVEFORMATEX SoundSystemDS::_getWaveFormat(const PCMSoundHeader& header, float pitch) const
{
	WAVEFORMATEX wf;

	wf.wFormatTag = WAVE_FORMAT_PCM;
	wf.nSamplesPerSec = DWORD(float(header.m_sample_rate) * pitch);
	wf.wBitsPerSample = 8 * header.m_bytes_per_sample;
	wf.nChannels = header.m_channels;
	wf.nBlockAlign = (wf.wBitsPerSample / 8) * wf.nChannels;
	wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
	wf.cbSize = 0;

	return wf;
}

// Release sounds that finished playing
void SoundSystemDS::_cleanSources()
{
	for (size_t i = 0; i < m_buffers.size(); i++)
	{
		DWORD status;
		m_buffers[i].buffer->GetStatus(&status);
		if (status != DSBSTATUS_PLAYING)
		{
			m_buffers[i].buffer->Release();
			if (m_buffers[i].object3d != NULL)
			{
				m_buffers[i].object3d->Release();
			}
			
			m_buffers.erase(m_buffers.begin() + i);
			i--;
		}
	}
}
