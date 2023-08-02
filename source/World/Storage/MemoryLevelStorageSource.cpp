/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "MemoryLevelStorage.hpp"
#include "MemoryLevelStorageSource.hpp"

#ifdef DEMO

std::string MemoryLevelStorageSource::getName()
{
	return "Memory Storage";
}

LevelStorage* MemoryLevelStorageSource::selectLevel(const std::string& x, bool y)
{
	return new MemoryLevelStorage;
}

void MemoryLevelStorageSource::clearAll()
{
}

int MemoryLevelStorageSource::getDataTagFor(const std::string& x)
{
	return 0;
}

bool MemoryLevelStorageSource::isNewLevelIdAcceptable(const std::string& x)
{
	return true;
}

void MemoryLevelStorageSource::deleteLevel(const std::string& x)
{
	LogMsg("Delete level: %s", x.c_str());
}

void MemoryLevelStorageSource::renameLevel(const std::string& x, const std::string& y)
{
}

bool MemoryLevelStorageSource::isConvertible(const std::string& x)
{
	return false;
}

bool MemoryLevelStorageSource::requiresConversion(const std::string& x)
{
	return false;
}

int MemoryLevelStorageSource::convertLevel(const std::string& x, ProgressListener* y)
{
	return 0;
}

#endif
