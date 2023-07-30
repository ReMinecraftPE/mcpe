/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

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

