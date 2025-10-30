/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once
#include <string>
#include <windows.h>
#include <uuids.h>
#include <strmif.h>
#include <stdexcept>
#include <vector>

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <mmsystem.h>
#include <dsound.h>

#include "client/sound/SoundSystem.hpp"
#include "client/sound/SoundData.hpp"

class SoundSystemDS : public SoundSystem
{
public:
	SoundSystemDS();
	~SoundSystemDS();
	virtual bool isAvailable() override;
	virtual void setListenerPos(const Vec3& pos) override;
	virtual void setListenerAngle(const Vec2& rot) override;
	virtual void playAt(const SoundDesc& sound, const Vec3& pos, float volume, float pitch) override;
private:
	WAVEFORMATEX _getWaveFormat(const PCMSoundHeader& header, float pitch) const;
	void _cleanSources();

	struct BufferInfo
	{
		LPDIRECTSOUNDBUFFER buffer;
		LPDIRECTSOUND3DBUFFER object3d;
	};

	bool m_available;
	HWND m_hWnd;
	IDirectSound* m_directsound;
	LPDIRECTSOUND3DLISTENER m_listener;
	std::vector<BufferInfo> m_buffers;
};

#define SOUND_SYSTEM SoundSystemDS
