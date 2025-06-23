/********************************************************************
    Minecraft: Pocket Edition - Decompilation Project
    Copyright (C) 2023 iProgramInCpp
    
    The following code is licensed under the BSD 1 clause license.
    SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SoundEngine.hpp"
#include "SoundDefs.hpp"
#include "common/Mth.hpp"
#include "world/entity/Mob.hpp"
#include "client/app/AppPlatform.hpp"

SoundEngine::SoundEngine(SoundSystem* soundSystem, float distance)
{
    m_pSoundSystem = soundSystem;
    m_pOptions = nullptr;
    field_40 = 0;
    m_listenerPosition = Vec3::ZERO;
    m_listenerOrientation = Vec2::ZERO;
    m_soundDistance = 1.0f / distance;
    m_noMusicDelay = m_random.nextInt(12000);
    field_A20 = 0;
    m_muted = false;
}

float SoundEngine::_getVolumeMult(const Vec3& pos)
{
    // Taken from 0.7.0. Very similar to paulscode.sound.libraries.SourceLWJGLOpenAL.calculateGain()
    float distance = 1.1f - (pos.distanceTo(m_listenerPosition) * m_soundDistance);
    return Mth::clamp(distance, -1.0f, 1.0f);
}

void SoundEngine::init(Options* options, AppPlatform* platform)
{
    // TODO: Who's the genius who decided it'd be better to check a name string rather than an enum?
    m_pOptions = options;
    // Load Sounds
    SoundDesc::_loadAll(platform);

#define SOUND(category, name, number) m_sounds.add(#category "." #name, SA_##name##number);
#include "sound_list.h"
#undef SOUND

#define MUSIC(name, number) m_songs.add(#name, platform->getAssetPath("music/" #name #number ".ogg"));
#define NEWMUSIC(name, number) m_songs.add(#name, platform->getAssetPath("newmusic/" #name #number ".ogg"));
#include "music_list.h"
#undef MUSIC
}

void SoundEngine::enable(bool b)
{
}

void SoundEngine::updateOptions()
{
}

void SoundEngine::mute()
{
    m_muted = true;
}

void SoundEngine::unMute()
{
    m_muted = false;
}

void SoundEngine::destroy()
{
    // Un-load Sounds
    SoundDesc::_unloadAll();
}

void SoundEngine::playMusicTick()
{
    if (m_pOptions->m_fMusicVolume <= 0.0f)
        return;

    if (!m_pSoundSystem->isPlayingMusic()/* && !soundSystem.playing("streaming")*/)
    {
        if (m_noMusicDelay > 0)
        {
            --m_noMusicDelay;
            return;
        }

        std::string songPath;
        if (m_songs.any(songPath))
        {
            m_noMusicDelay = m_random.nextInt(12000) + 12000;
            m_pSoundSystem->setMusicVolume(m_pOptions->m_fMusicVolume);
            m_pSoundSystem->playMusic(songPath);
        }
    }
}


void SoundEngine::update(const Mob* player, float elapsedTime)
{
    if (m_pOptions->m_fMasterVolume > 0.0f)
    {
        if (player != nullptr)
        {
            Vec3 pos = player->getPos(elapsedTime);
            pos.y -= player->m_heightOffset;
            m_listenerPosition = pos;
            m_pSoundSystem->setListenerPos(pos);

            Vec2 rot = player->getRot(elapsedTime);
            m_listenerOrientation = rot;
            m_pSoundSystem->setListenerAngle(rot);
        }
    }

    assert(m_pSoundSystem->isAvailable());

    m_pSoundSystem->update(elapsedTime);
}

void SoundEngine::play(const std::string& name, const Vec3& pos, float volume, float pitch)
{
    float vol = m_pOptions->m_fMasterVolume * volume;
    if (vol <= 0.0f)
        return;
    Vec3 nPos;
    float distance = pos.distanceTo(m_listenerPosition);
    if (distance > SOUND_MAX_DISTANCE)
        return;
    // @HACK: Annoying hack because DirectSound is making steps in 2D insanely quiet.
#ifdef USE_OPENAL
    if (distance < SOUND_ATTENUATION_MIN_DISTANCE)
        nPos = Vec3::ZERO;
    else
        nPos = pos;
#else
    nPos = pos;
#endif

    float cVolume = Mth::clamp(_getVolumeMult(pos) * vol, 0.0f, 1.0f);
    float cPitch = Mth::clamp(pitch, 0.5f, 2.0f); // Clamp to values specified by Paulscode
    SoundDesc sd;

    if (m_sounds.get(name, sd))
    {
        m_pSoundSystem->playAt(sd, nPos, cVolume, cPitch);
    }
}

void SoundEngine::playUI(const std::string& name, float volume, float pitch)
{
    volume *= 0.25F; // present on Java b1.2_02, but not Pocket for some reason
    float vol = m_pOptions->m_fMasterVolume * volume;
    if (vol <= 0.0f)
        return;

    float cVolume = Mth::clamp(vol, 0.0f, 1.0f);
    SoundDesc sd;

    if (m_sounds.get(name, sd))
    {
        m_pSoundSystem->playAt(sd, Vec3::ZERO, cVolume, pitch);
    }
}

void SoundEngine::playMusic(const std::string& name)
{
    float vol = m_pOptions->m_fMusicVolume;
    if (vol <= 0.0f)
        return;

    std::string path;

    if (m_songs.get(name, path))
    {
        m_pSoundSystem->setMusicVolume(vol);
        m_pSoundSystem->playMusic(path);
    }
}
