/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <set>
#include <sstream>
#include "LevelStorageSource.hpp"

#ifndef DEMO

class ExternalFileLevelStorageSource : public LevelStorageSource
{
public:
	ExternalFileLevelStorageSource(const std::string& path);

	std::string getName() const override;
	LevelStorage* selectLevel(const std::string& name, bool b, bool forceConversion) override;
	void getLevelList(std::vector<LevelSummary>&) override;
	void clearAll() override;
	int getDataTagFor(const std::string&) override;
	bool isNewLevelIdAcceptable(const std::string&) override;
	void deleteLevel(const std::string&) override;
	void renameLevel(const std::string&, const std::string&) override;
	bool isConvertible(const std::string&) override;
	bool requiresConversion(const std::string&) override;
	int convertLevel(const std::string&, ProgressListener*) override;

	void addLevelSummaryIfExists(std::vector<LevelSummary>& vls, const char* name);

public:
	std::string m_worldsPath;
};

#endif
