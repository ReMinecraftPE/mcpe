#pragma once

#include "../AreaNavigation.hpp"
#include "../GuiComponent.hpp"
#include "common/Mth.hpp"

class ScrollRenderer_Console : public GuiComponent
{
public:
    ScrollRenderer_Console();

public:
    void updateScroll(AreaNavigation::Direction);
    void renderScroll(AreaNavigation::Direction, Textures&, int x, int y);
    void renderScroll(AreaNavigation::Direction, Textures&, int x, int y, const std::string&, int width, int height);

    static float getAlpha(int last)
    {
        float f = (getTimeMs() - last) / 320.0f;
        return Mth::Min(1.0f, f < 0.5f ? 1 - f * 2.0f : (f - 0.5f) * 2.0f);
    }

public:
    int m_lastScrolled[4];

    static std::string SCROLLS[];
};