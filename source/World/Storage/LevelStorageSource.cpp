#include "LevelStorageSource.hpp"

LevelStorageSource::~LevelStorageSource()
{
}

void LevelStorageSource::getLevelList(std::vector<LevelSummary>& vec)
{
	// @TODO: complete mock
#ifndef ORIGINAL_CODE
	vec.push_back(LevelSummary("Level1", "Level-1", 12345, 1234567));
	vec.push_back(LevelSummary("Level2", "Level-2", 23456, 2345678));
	vec.push_back(LevelSummary("Level3", "Level-3", 34567, 3456789));
	vec.push_back(LevelSummary("Level4", "Level-4", 45678, 4567890));
	vec.push_back(LevelSummary("Level5", "Level-5", 56789, 5678901));
#endif
}
