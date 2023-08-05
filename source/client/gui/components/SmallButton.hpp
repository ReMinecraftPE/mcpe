/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Button.hpp"

class SmallButton : public Button
{
public:
	SmallButton(int id, int x, int y, const std::string& str);
	SmallButton(int id, int x, int y, int width, int height, const std::string& str);
	SmallButton(int id, int x, int y, Options::Option* pOption, const std::string& str);
	Options::Option* getOption();

private:
	Options::Option* m_pOption = nullptr;
};

