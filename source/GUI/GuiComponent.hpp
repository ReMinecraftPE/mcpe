#pragma once

#include "Tesselator.hpp"
#include "Font.hpp"

class GuiComponent
{
public:
	void blit(int dstX, int dstY, int srcX, int srcY, int dstWidth, int dstHeight, int srcWidth, int srcHeight);
	void drawCenteredString(Font*, const std::string&, int cx, int cy, int color);
	void drawString(Font*, const std::string&, int cx, int cy, int color);
	void fill(int left, int top, int right, int bottom, int color);
	void fillGradient(int left, int top, int right, int bottom, int colorUp, int colorDown);

public:
	float field_4;
};

