/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "LevelStorageSource.hpp"

#ifdef DEMO
class MemoryLevelStorageSource : public LevelStorageSource
{
	std::string getName() override;
	LevelStorage* selectLevel(const std::string&, bool) override;
	void clearAll() override;
	int getDataTagFor(const std::string&) override;
	bool isNewLevelIdAcceptable(const std::string&) override;
	void deleteLevel(const std::string&) override;
	void renameLevel(const std::string&, const std::string&) override;
	bool isConvertible(const std::string&) override;
	bool requiresConversion(const std::string&) override;
	int convertLevel(const std::string&, ProgressListener*) override;
};
#endif
