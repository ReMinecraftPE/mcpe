#pragma once

#include <string>
#include "SoundData.hpp"

class SoundSystem
{
public:
	virtual bool isAvailable();
	virtual void setListenerPos(float x, float y, float z);
	virtual void setListenerAngle(float f);
	virtual void load(const std::string& sound);
	virtual void play(const std::string& sound);
	virtual void pause(const std::string& sound);
	virtual void stop(const std::string& sound);
	virtual void playAt(const SoundDesc& sound, float x, float y, float z, float a, float b);
};

