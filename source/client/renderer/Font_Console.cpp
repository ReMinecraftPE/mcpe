#include "client/renderer/Font_Console.hpp"

Font_Console::Font_Console(Options* options, const std::string& fileName, Textures* textures)
	: Font(options, fileName, textures)
{
	m_unicodeShadowOffset = 1.0f;
}

Font_Console::~Font_Console()
{
}

void Font_Console::drawScalableShadow(const std::string& str, int x, int y, const Color& color, float scale)
{
	{
		MatrixStack::Ref matrix = MatrixStack::World.push();
		matrix->translate(Vec3(x - 1, y - 1, 0));
		matrix->scale(scale);
		draw(str, 0, 0, color, true);
	}

	drawScalable(str, x, y, color, scale, false);
}

void Font_Console::drawString(const std::string& str, int x, int y, const Color& color, bool hasShadow)
{
	if (hasShadow)
	{
		drawScalableShadow(str, x, y, color, 2.0f);
	}
	else
	{
		drawScalable(str, x, y, color);
	}
}
