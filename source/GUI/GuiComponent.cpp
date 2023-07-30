/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "GuiComponent.hpp"

#ifdef _WIN32
#pragma warning (disable : 4244)
#endif

void GuiComponent::blit(int dx, int dy, int sx, int sy, int tw, int th, int sw, int sh)
{
	auto& t = Tesselator::instance;

	if (!sh) sh = th;
	if (!sw) sw = tw;

	int width = sw, height = sh;

	t.begin();
	t.vertexUV(dx,      dy + th, field_4, float(sx)      / 256.0f, float(sy + sh) / 256.0f);
	t.vertexUV(dx + tw, dy + th, field_4, float(sx + sw) / 256.0f, float(sy + sh) / 256.0f);
	t.vertexUV(dx + tw, dy,      field_4, float(sx + sw) / 256.0f, float(sy)      / 256.0f);
	t.vertexUV(dx,      dy,      field_4, float(sx)      / 256.0f, float(sy)      / 256.0f);
	t.draw();
}

void GuiComponent::drawCenteredString(Font* pFont, const std::string& str, int cx, int cy, int color)
{
	int width = pFont->width(str);
	int height = pFont->height(str);
	pFont->drawShadow(str, cx - width / 2, cy - height / 2, color);
}

void GuiComponent::drawString(Font* pFont, const std::string& str, int cx, int cy, int color)
{
	pFont->drawShadow(str, cx, cy, color);
}

void GuiComponent::fill(int a2, int a3, int a4, int a5, int a6)
{
	glEnable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(float(GET_RED(a6)) / 255.0f, float(GET_GREEN(a6)) / 255.0f, float(GET_BLUE(a6)) / 255.0f, float(GET_ALPHA(a6)) / 255.0f);

	Tesselator& t = Tesselator::instance;
	t.begin();

	t.vertex(a2, a5, 0.0f);
	t.vertex(a4, a5, 0.0f);
	t.vertex(a4, a3, 0.0f);
	t.vertex(a2, a3, 0.0f);

	t.draw();

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void GuiComponent::fillGradient(int a2, int a3, int a4, int a5, int a6, int a7)
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_SMOOTH);

	Tesselator& t = Tesselator::instance;
	t.begin();

	// note: for some stupid reason OG uses the float overload.
	t.color(float(GET_RED(a6)) / 255.0f, float(GET_GREEN(a6)) / 255.0f, float(GET_BLUE(a6)) / 255.0f, float(GET_ALPHA(a6)) / 255.0f);
	t.vertex(a2, a5, 0.0f);
	t.vertex(a4, a5, 0.0f);
	t.color(float(GET_RED(a7)) / 255.0f, float(GET_GREEN(a7)) / 255.0f, float(GET_BLUE(a7)) / 255.0f, float(GET_ALPHA(a7)) / 255.0f);
	t.vertex(a4, a3, 0.0f);
	t.vertex(a2, a3, 0.0f);

	t.draw();

	glShadeModel(GL_FLAT);
	glDisable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_TEXTURE_2D);
}
