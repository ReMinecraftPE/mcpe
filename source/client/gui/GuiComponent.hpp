/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "client/renderer/ScreenRenderer.hpp"
#include "client/renderer/Font.hpp"


class GuiComponent : public ScreenRenderer
{
public:
	virtual ~GuiComponent() {}

	void drawCenteredString(Font*, const std::string&, int cx, int cy, const Color& color);
	void drawString(Font*, const std::string&, int cx, int cy, const Color& color);
	void fillGradient(int left, int top, int right, int bottom, const Color& colorUp, const Color& colorDown);
};

