#pragma once

#include "LevelStorageSource.hpp"

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
	void convertLevel(const std::string&, ProgressListener*) override;
};

