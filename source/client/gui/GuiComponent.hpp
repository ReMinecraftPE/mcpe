/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "client/renderer/Tesselator.hpp"
#include "client/renderer/Font.hpp"

class GuiComponent
{
public:
	void blit(int dstX, int dstY, int srcX, int srcY, int dstWidth, int dstHeight, int srcWidth, int srcHeight);
	void drawCenteredString(Font*, const std::string&, int cx, int cy, int color);
	void drawString(Font*, const std::string&, int cx, int cy, int color);
	void fill(int left, int top, int right, int bottom, int color);
	void fillGradient(int left, int top, int right, int bottom, int colorUp, int colorDown);

public:
	float field_4 = 0;
};

