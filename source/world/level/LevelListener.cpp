/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LevelListener.hpp"

void LevelListener::tileChanged(const TilePos& pos)
{

}

void LevelListener::tileBrightnessChanged(const TilePos& pos)
{
	tileChanged(pos);
}

void LevelListener::setTilesDirty(const TilePos& min, const TilePos& max)
{

}

void LevelListener::allChanged()
{

}

void LevelListener::playSound(const std::string& name, const Vec3& pos, float volume, float pitch)
{

}

void LevelListener::takePicture(TripodCamera*, Entity*)
{

}

void LevelListener::addParticle(const std::string& a, const Vec3& pos, const Vec3& dir)
{

}

void LevelListener::playMusic(const std::string& a, float b, float c, float d, float e)
{

}

void LevelListener::entityAdded(Entity* a)
{

}

void LevelListener::entityRemoved(Entity* a)
{

}

void LevelListener::skyColorChanged()
{

}

void LevelListener::timeChanged(uint32_t time)
{

}

void LevelListener::playStreamingMusic(const std::string& a, int b, int c, int d)
{

}
