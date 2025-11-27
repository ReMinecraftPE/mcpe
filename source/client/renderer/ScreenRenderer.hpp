#pragma once

#include "common/math/Color.hpp"
#include "client/gui/IntRectangle.hpp"
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
	void fill(int left, int top, int right, int bottom, const Color& color);

private:
	Materials m_materials;
protected:
	float m_blitOffset;
};

