#include <math.h>

#include "Color.hpp"

Color Color::SHADE_WEST_EAST   = Color(0.6f, 0.6f, 0.6f);
Color Color::SHADE_NORTH_SOUTH = Color(0.8f, 0.8f, 0.8f);
Color Color::SHADE_UP          = Color(1.0f, 1.0f, 1.0f);
Color Color::SHADE_DOWN        = Color(0.5f, 0.5f, 0.5f);

Color Color::NIL    = Color(0.0f, 0.0f, 0.0f, 0.0f);
Color Color::CYAN   = Color(0.0f, 1.0f, 1.0f);
Color Color::PURPLE = Color(1.0f, 0.0f, 1.0f);
Color Color::YELLOW = Color(1.0f, 1.0f, 0.0f);
Color Color::BLUE   = Color(0.0f, 0.0f, 1.0f);
Color Color::GREEN  = Color(0.0f, 1.0f, 0.0f);
Color Color::RED    = Color(1.0f, 0.0f, 0.0f);
Color Color::BLACK  = Color(0.0f, 0.0f, 0.0f);
Color Color::GREY   = Color(0.5f, 0.5f, 0.5f);
Color Color::WHITE  = Color(1.0f, 1.0f, 1.0f);

Color Color::TEXT_GREY = Color(0x404040);

void Color::fromHSB(float h, float s, float b)
{
    if (s == 0.0f)
    {
        this->r = b;
        this->g = b;
        this->b = b;
        this->a = 1.0f;
    }
    else
    {
        float v7 = (h - floorf(h)) * 6.0f;
        float v10 = (1.0f - s) * b;
        float v11 = v7 - floorf(v7);
        float v12 = (1.0f - (v11 * s)) * b;
        float v13 = b + (((v11 * s) - s) * b);
        switch ((int)v7)
        {
        case 0:
            this->r = b;
            this->g = v13;
            this->b = v10;
            this->a = 1.0f;
            break;
        case 1:
            this->r = v12;
            this->g = b;
            this->b = v10;
            this->a = 1.0f;
            break;
        case 2:
            this->r = v10;
            this->g = b;
            this->b = v13;
            this->a = 1.0f;
            break;
        case 3:
            this->r = v10;
            this->g = v12;
            this->b = b;
            this->a = 1.0f;
            break;
        case 4:
            this->r = v13;
            this->g = v10;
            this->b = b;
            this->a = 1.0f;
            break;
        case 5:
            this->r = b;
            this->g = v10;
            this->b = v12;
            this->a = 1.0f;
            break;
        default:
            *this = Color::NIL;
            break;
        }
    }
}
