#pragma once

#include "renderer/MaterialPtr.hpp"
#include "client/gui/IntRectangle.hpp"

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

private:
	Materials m_materials;
};

