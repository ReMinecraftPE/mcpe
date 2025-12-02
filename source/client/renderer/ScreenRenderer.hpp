#pragma once

#include "common/math/Color.hpp"
#include "client/gui/IntRectangle.hpp"
#include "client/renderer/Font.hpp"
#include "renderer/MaterialPtr.hpp"

class ScreenRenderer
{
protected:
	class Materials
	{
	public:
		mce::MaterialPtr ui_textured;
		mce::MaterialPtr ui_fill_color;
		mce::MaterialPtr ui_fill_gradient;
		mce::MaterialPtr ui_texture_and_color;
		mce::MaterialPtr ui_textured_and_glcolor;
		mce::MaterialPtr ui_fill_stencil;

		Materials();
	};

private:
	static ScreenRenderer* singletonPtr;
public:
	static ScreenRenderer& singleton();

public:
	ScreenRenderer();

public:
	void blit(const IntRectangle& rect);
	void blit(int dstX, int dstY, int srcX, int srcY, int dstWidth, int dstHeight, int srcWidth, int srcHeight, mce::MaterialPtr* materialPtr = nullptr);
	void blitRaw(float x1, float x2, float y1, float y2, float z, float u1, float u2, float v1, float v2);
	void drawCenteredString(Font& font, const std::string& str, int cx, int cy, const Color& color);
	void drawString(Font& font, const std::string& str, int cx, int cy, const Color& color);
	void fill(float left, float top, float right, float bottom, const Color& color);
	void fill(int left, int top, int right, int bottom, const Color& color);
	void fillGradient(float left, float top, float right, float bottom, const Color& colorUp, const Color& colorDown);
	void fillGradient(int left, int top, int right, int bottom, const Color& colorUp, const Color& colorDown);


protected:
	Materials m_materials;
	float m_blitOffset;
};

