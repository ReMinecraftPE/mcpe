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

#ifdef _MSC_VER
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#endif

#include <mmsystem.h>
#include <dsound.h>

#include "client/sound/SoundSystem.hpp"
#include "client/sound/SoundData.hpp"
#include "SoundStreamDS.hpp"

class SoundSystemDS : public SoundSystem
{
public:
	SoundSystemDS();
	~SoundSystemDS();
	virtual bool isAvailable() override;
	virtual void setListenerPos(const Vec3& pos) override;
	virtual void setListenerAngle(const Vec2& rot) override;
	virtual void playAt(const SoundDesc& sound, const Vec3& pos, float volume, float pitch) override;
	virtual void update(float) override;
	virtual void playMusic(const std::string& soundPath) override;
	virtual bool isPlayingMusic() const override;
	virtual void stopMusic() override;
	virtual void pauseMusic(bool state) override;
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
	SoundStreamDS* _musicStream;
};

#define SOUND_SYSTEM SoundSystemDS
