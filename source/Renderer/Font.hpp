#pragma once

#include "Textures.hpp"
#include "Options.hpp"

class Font
{
public:
	Font(Options* pOpts, const std::string& fileName, Textures* pTexs);

	void init(Options* pOpts);
	void buildChar(unsigned char chr, float x, float y);
	void draw(const std::string&, int x, int y, int color);
	void draw(const std::string&, int x, int y, int color, bool bShadow);
	void drawSlow(const std::string&, int x, int y, int color, bool bShadow);
	void drawShadow(const std::string&, int x, int y, int color);

	//int Font::drawWordWrap(Font *this, const StlString *a2, int a3, int a4, int a5, int a6).
	// +- I probably won't actually implement this because
	// +- 1. It does not seem to have any cross references
	// +- 2. It appears to even be broken

	void onGraphicsReset();

	int width(const std::string& str);
	int height(const std::string& str);

private:
	int field_0 = 0; 
	int m_charWidthInt[256];
	float m_charWidthFloat[256];
	// huge gap, don't know why it's there...
	std::string m_fileName;
	Options* m_pOptions;
	Textures* m_pTextures;
};

