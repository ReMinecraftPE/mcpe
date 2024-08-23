/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "Button.hpp"

class ImageButton : public Button
{
public:
	ImageButton(int id, int x, int y, const std::string& str);
	ImageButton(int id, int x, int y, int width, int height, const std::string& str);
	ImageButton(int id, const std::string& str);
	//ImageButton(int id, int param_1, const std::string& str, ImageDef *param_3);
	//void setImageDef(ImageDef* param_1, bool param_2);
private:
};

