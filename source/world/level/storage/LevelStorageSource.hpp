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

	LevelSummary()
	{
		m_lastPlayed = 0;
		m_sizeOnDisk = 0;
	}

	LevelSummary(const std::string& a, const std::string& b, int c, int d)
	{
		m_fileName = a;
		m_levelName = b;
		m_lastPlayed = c;
		m_sizeOnDisk = d;
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
	virtual std::string getName() = 0;
	virtual LevelStorage* selectLevel(const std::string&, bool) = 0;
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

