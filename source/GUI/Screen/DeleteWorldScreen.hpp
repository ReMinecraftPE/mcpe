#pragma once

#include "ConfirmScreen.hpp"

class DeleteWorldScreen : public ConfirmScreen
{
public:
	DeleteWorldScreen(const LevelSummary& level);

	void postResult(bool b) override;

private:
	LevelSummary m_level;
};

