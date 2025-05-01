/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "client/options/Options.hpp"
#include "common/Random.hpp"
#include "world/phys/Vec3.hpp"
#include "world/phys/Vec2.hpp"
#include "SoundSystem.hpp"
#include "SoundRepository.hpp"
#include "SoundPathRepository.hpp"
#include "SoundStream.hpp"

class Mob;

class SoundEngine
{
private:
    float _getVolumeMult(const Vec3& pos);
public:
    SoundEngine(SoundSystem* soundSystem, float distance);
    void init(Options*, AppPlatform*);
    void enable(bool b);
    void updateOptions();
    void mute();
    void unMute();
    void destroy();
    void playMusicTick();
    void update(const Mob* player, float elapsedTime);
    void play(const std::string& name, const Vec3& pos = Vec3::ZERO, float volume = 1.0f, float pitch = 1.0f);
    void playUI(const std::string& name, float volume = 1.0f, float pitch = 1.0f);
    void playMusic(const std::string& name);

public:
    SoundSystem* m_pSoundSystem;
private:
    SoundRepository m_sounds;
    SoundPathRepository m_songs;
    Options* m_pOptions;
    int field_40;
    Random m_random;
    Vec3 m_listenerPosition;
    Vec2 m_listenerOrientation;
    float m_soundDistance;
    int m_noMusicDelay;
    int field_A20;
    bool m_muted;
    //std::vector<SoundStream> m_streams;
};

