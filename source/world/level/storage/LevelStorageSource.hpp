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
	std::string field_0;
	std::string field_18;
	int field_30 = 0;
	int field_34 = 0;

	LevelSummary() {}

	LevelSummary(const std::string& a, const std::string& b, int c, int d)
	{
		field_0 = a;
		field_18 = b;
		field_30 = c;
		field_34 = d;
	}

	bool operator<(const LevelSummary& b) const
	{
		// @NOTE: I *think* that's how it works. Not entirely sure.
		return field_30 > b.field_30;
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

