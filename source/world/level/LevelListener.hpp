/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include "world/entity/TripodCamera.hpp"

class LevelListener
{
public:
	virtual ~LevelListener() {}
	virtual void tileChanged(const TilePos& pos);
	virtual void tileBrightnessChanged(const TilePos& pos);
	virtual void setTilesDirty(const TilePos& min, const TilePos& max);
	virtual void allChanged();
	virtual void playSound(const std::string&, const Vec3& pos, float, float);
	virtual void takePicture(TripodCamera*, Entity*);
	virtual void addParticle(const std::string&, const Vec3& pos, const Vec3& dir);
	virtual void playMusic(const std::string&, float, float, float, float);
	virtual void entityAdded(Entity*);
	virtual void entityRemoved(Entity*);
	virtual void skyColorChanged();
	virtual void timeChanged(uint32_t time);
	virtual void playStreamingMusic(const std::string&, int, int, int);
};

