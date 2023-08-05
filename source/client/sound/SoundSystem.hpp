/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include "SoundData.hpp"

class SoundSystem
{
public:
	virtual bool isAvailable();
	virtual void setListenerPos(float x, float y, float z);
#ifndef ORIGINAL_CODE
	virtual void setListenerAngle(float yaw, float pitch);
#else
	virtual void setListenerAngle(float yaw);
#endif
	virtual void load(const std::string& sound);
	virtual void play(const std::string& sound);
	virtual void pause(const std::string& sound);
	virtual void stop(const std::string& sound);
	virtual void playAt(const SoundDesc& sound, float x, float y, float z, float a, float b);
};

