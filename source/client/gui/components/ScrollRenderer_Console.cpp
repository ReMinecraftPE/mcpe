#include <cstring>

#include "ScrollRenderer_Console.hpp"
#include "renderer/ShaderConstants.hpp"

std::string ScrollRenderer_Console::SCROLLS[] = { "gui/console/scrollUp.png", "gui/console/scrollDown.png", "gui/console/scrollLeft.png", "gui/console/scrollRight.png" };

ScrollRenderer_Console::ScrollRenderer_Console()
{
	memset(m_lastScrolled, 0, sizeof(m_lastScrolled));
}

void ScrollRenderer_Console::updateScroll(AreaNavigation::Direction dir)
{
	m_lastScrolled[dir] = getTimeMs();
}

void ScrollRenderer_Console::renderScroll(AreaNavigation::Direction dir, Textures& texs, int x, int y)
{
    switch (dir)
    {
    case AreaNavigation::LEFT: case AreaNavigation::RIGHT:
        renderScroll(dir, texs, x, y, SCROLLS[dir], 26, 22);
        break;
    case AreaNavigation::UP: case AreaNavigation::DOWN:
        renderScroll(dir, texs, x, y, SCROLLS[dir], 32, 22);
        break;
    }
}

void ScrollRenderer_Console::renderScroll(AreaNavigation::Direction dir, Textures& texs, int x, int y, const std::string& sprite, int width, int height)
{
    int l = m_lastScrolled[dir];
    if (l > 0)
        currentShaderColor = Color(1.0f, 1.0f, 1.0f, getAlpha(l));
    blitSprite(texs, sprite, x, y, width, height, &m_materials.ui_textured_and_glcolor);
    if (l > 0)
        currentShaderColor = Color::WHITE;
}
