#pragma once

#include "ConfirmScreen.hpp"
#include "world/level/storage/LevelStorageSource.hpp"

class ConvertWorldScreen : public ConfirmScreen
{
public:
	ConvertWorldScreen(const LevelSummary& level);
    
	void postResult(bool b) override;
    
private:
	LevelSummary m_level;
};

