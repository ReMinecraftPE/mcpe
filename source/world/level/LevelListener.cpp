/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LevelListener.hpp"

void LevelListener::tileChanged(int x, int y, int z)
{

}

void LevelListener::tileBrightnessChanged(int x, int y, int z)
{
	tileChanged(x, y, z);
}

void LevelListener::setTilesDirty(int a, int b, int c, int d, int e, int f)
{

}

void LevelListener::allChanged()
{

}

void LevelListener::playSound(const std::string& a, float b, float c, float d, float e, float f)
{

}

void LevelListener::takePicture(TripodCamera*, Entity*)
{

}

void LevelListener::addParticle(const std::string& a, float b, float c, float d, float e, float f, float g)
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

void LevelListener::playStreamingMusic(const std::string& a, int b, int c, int d)
{

}
