/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include "world/phys/Vec2.hpp"
#include "world/phys/Vec3.hpp"
#include "SoundData.hpp"

// Platform-agnostic sound settings //
// Just guessing for this one
#define SOUND_ATTENUATION_MIN_DISTANCE 2.0f
#define SOUND_MAX_DISTANCE 16.0f
// 28 non-streaming channels in Paulscode
// @NOTE: Currently only SoundSystemAL adheres to this.
#define SOUND_MAX_SOURCES 28

class SoundSystem
{
public:
	virtual ~SoundSystem();

	virtual bool isAvailable();
	virtual void setListenerPos(const Vec3& pos);
	virtual void setListenerAngle(const Vec2& rot);
	virtual void load(const std::string& sound);
	virtual void play(const std::string& sound);
	virtual void pause(const std::string& sound);
	virtual void stop(const std::string& sound);
	virtual void playAt(const SoundDesc& sound, const Vec3& pos, float volume, float pitch);
    
    // Be prepared for these to be called regardless of engine state
    virtual void startEngine(); // called init in 0.10.0
    virtual void stopEngine(); // called destroy in 0.10.0
    
    virtual void muteAudio();
    virtual void unMuteAudio();
};

