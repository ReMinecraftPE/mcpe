/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include <vector>
#include <list>

#define USE_PTHREAD_MUTEX

#ifdef USE_PTHREAD_MUTEX
#include <pthread.h>
#else
#error TODO: Implement support for other types of mutexes, such
#error as C++11 mutexes. Although I'm pretty sure this will only
#error be included for Android. -iProgram
#endif

#include <SLES/OpenSLES.h>
#ifdef ANDROID
#include <SLES/OpenSLES_Android.h>
#endif

#include "client/sound/SoundSystem.hpp"
#include "client/sound/SoundData.hpp"

typedef std::list<SLObjectItf> SLSoundList;

#define SOUND_SYSTEM SoundSystemSL

class SoundSystemSL : public SoundSystem
{
public:
	SoundSystemSL();
	~SoundSystemSL();
	void init();
	void destroy();
	bool checkErr(SLresult res);
	void removeStoppedSounds();
	void setListenerPos(const Vec3& pos) override;
	void setListenerAngle(const Vec2& rot) override;
	void playAt(const SoundDesc& sound, const Vec3& pos, float volume, float pitch) override;
	static void removePlayer(SLAndroidSimpleBufferQueueItf caller, void* context);
private:
	SLSoundList m_playingSounds;
	SLEngineItf m_slEngine;
	SL3DLocationItf m_3dLocationItf;
	SLObjectItf m_slOutputMix;
	Vec3 m_listenerPos;
	int m_soundCount;
	bool m_bAvailable;
	std::vector<SLObjectItf> m_tempToRemove;

	static std::vector<SLObjectItf> toRemove;
	static SLObjectItf objEngine;
	static pthread_mutex_t toRemoveMutex;
};
