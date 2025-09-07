/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "LevelStorage.hpp"

struct LevelSummary
{
	std::string m_fileName;
	std::string m_levelName;
	int m_lastPlayed;
	int m_sizeOnDisk;
	GameType m_gameType;
    unsigned int m_storageVersion;

	LevelSummary()
	{
		m_lastPlayed = 0;
		m_sizeOnDisk = 0;
		m_gameType = GAME_TYPE_CREATIVE;
        m_storageVersion = LEVEL_STORAGE_VERSION_DEFAULT;
	}

	LevelSummary(const std::string& fileName, const std::string& levelName, int lastPlayed, int sizeOnDisk, GameType gameType = GAME_TYPE_CREATIVE, unsigned int storageVersion = LEVEL_STORAGE_VERSION_DEFAULT)
	{
		m_fileName = fileName;
		m_levelName = levelName;
		m_lastPlayed = lastPlayed;
		m_sizeOnDisk = sizeOnDisk;
		m_gameType = gameType;
        m_storageVersion = storageVersion;
	}

	bool operator<(const LevelSummary& b) const
	{
		return m_lastPlayed > b.m_lastPlayed;
	}
};

class LevelStorageSource
{
public:
	virtual ~LevelStorageSource();
	virtual std::string getName() const = 0;
	virtual LevelStorage* selectLevel(const std::string& name, bool b, bool forceConversion = false) = 0;
	virtual void getLevelList(std::vector<LevelSummary>&);
	virtual void clearAll() = 0;
	virtual int getDataTagFor(const std::string&) = 0;
	virtual bool isNewLevelIdAcceptable(const std::string&) = 0;
	virtual void deleteLevel(const std::string&) = 0;
	virtual void renameLevel(const std::string&, const std::string&) = 0;
	virtual bool isConvertible(const std::string&) = 0;
	virtual bool requiresConversion(const std::string&) = 0;
	virtual int convertLevel(const std::string&, ProgressListener*) = 0;
};

