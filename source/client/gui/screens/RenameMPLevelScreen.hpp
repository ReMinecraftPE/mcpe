/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "../Screen.hpp"

class RenameMPLevelScreen : public Screen
{
public:
	RenameMPLevelScreen(const std::string& levelName);
	void init() override;
	void render(int mouseX, int mouseY, float f) override;

private:
	std::string m_levelName;
};

