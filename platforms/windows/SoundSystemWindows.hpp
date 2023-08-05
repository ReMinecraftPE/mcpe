/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once
#include <string>
#include <Windows.h>
#include <uuids.h>
#include <strmif.h>
#include <stdexcept>
#include <vector>

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <mmsystem.h>
#include <dsound.h>

#include "client/sound/SoundData.hpp"

class SoundSystemWindows
{
public:
	SoundSystemWindows();
	~SoundSystemWindows();
	virtual bool isAvailable();
	virtual void setListenerPos(float x, float y, float z);
	virtual void setListenerAngle(float yaw, float pitch);
	virtual void load(const std::string& sound);
	virtual void play(const std::string& sound);
	virtual void pause(const std::string& sound);
	virtual void stop(const std::string& sound);
	virtual void playAt(const SoundDesc& sound, float x, float y, float z, float a, float b);
private:
	bool m_available = false;
	IDirectSound8* m_directsound;
	LPDIRECTSOUND3DLISTENER8 m_listener;
	std::vector<LPDIRECTSOUNDBUFFER> m_buffers;
};