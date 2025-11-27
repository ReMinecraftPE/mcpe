/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "GuiComponent.hpp"
#include "client/renderer/Tesselator.hpp"

void GuiComponent::drawCenteredString(Font* pFont, const std::string& str, int cx, int cy, const Color& color)
{
	int width = pFont->width(str);
	int height = pFont->height(str);
	pFont->drawShadow(str, cx - width / 2, cy - height / 2, color);
}

void GuiComponent::drawString(Font* pFont, const std::string& str, int cx, int cy, const Color& color)
{
	pFont->drawShadow(str, cx, cy, color);
}

void GuiComponent::fillGradient(int left, int top, int right, int bottom, const Color& colorUp, const Color& colorDown)
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_SMOOTH);

	Tesselator& t = Tesselator::instance;
	t.begin();

	t.color(colorUp);
	t.vertex(left, bottom, 0.0f);
	t.vertex(right, bottom, 0.0f);
	t.color(colorDown);
	t.vertex(right, top, 0.0f);
	t.vertex(left, top, 0.0f);

	t.draw();

	glShadeModel(GL_FLAT);
	glDisable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_TEXTURE_2D);
}
