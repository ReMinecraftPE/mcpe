#pragma once

#include "renderer/MaterialPtr.hpp"
#include "client/gui/IntRectangle.hpp"

class ScreenRenderer
{
private:
	static ScreenRenderer* singletonPtr;
public:
	static ScreenRenderer& singleton();

public:
	ScreenRenderer();

public:
	void blit(const IntRectangle& rect);

private:
	mce::MaterialPtr m_matBlit;
	mce::MaterialPtr m_matFill;
	mce::MaterialPtr m_matFillGradient;
	mce::MaterialPtr m_matColorBlit;
	mce::MaterialPtr m_matColorBlitBlur;
	mce::MaterialPtr m_matStencilFill;
};

