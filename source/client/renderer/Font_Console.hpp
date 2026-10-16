#pragma once
#include "client/renderer/Font.hpp"

class Font_Console : public Font
{
public:
	Font_Console(Options* options, const std::string& fileName, Textures* textures);
	virtual ~Font_Console();

	void drawScalableShadow(const std::string& str, int x, int y, const Color& color, float scale) override;
	void drawString(const std::string& str, int x, int y, const Color& color, bool hasShadow) override;
};
