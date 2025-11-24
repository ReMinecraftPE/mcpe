#pragma once

#include <stdint.h>

#define GET_RED(c)   (uint8_t(((c) >>  0) & 0xFF))
#define GET_GREEN(c) (uint8_t(((c) >>  8) & 0xFF))
#define GET_BLUE(c)  (uint8_t(((c) >> 16) & 0xFF))
#define GET_ALPHA(c) (uint8_t(((c) >> 24) & 0xFF))

struct Color
{
public:
    float r;
    float g;
    float b;
    float a;

private:
    void _init(float r, float g, float b, float a)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    void _init(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        this->r = float(r) / 255.0f;
        this->g = float(g) / 255.0f;
        this->b = float(b) / 255.0f;
        this->a = float(a) / 255.0f;
    }

public:
    Color()
    {
		*this = NIL;
    }

    Color(float r, float g, float b, float a = 1.0f)
    {
        _init(r, g, b, a);
    }

    Color(int r, int g, int b, int a = 255)
    {
        _init((uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a);
    }

    Color(unsigned int c)
    {
        _init(GET_RED(c), GET_GREEN(c), GET_BLUE(c), GET_ALPHA(c));
    }

    void fromHSB(float h, float s, float b);

public:
    static Color SHADE_WEST_EAST;
    static Color SHADE_NORTH_SOUTH;
    static Color SHADE_UP;
    static Color SHADE_DOWN;

    static Color NIL;
    static Color CYAN;
    static Color PURPLE;
    static Color YELLOW;
    static Color BLUE;
    static Color GREEN;
    static Color RED;
    static Color BLACK;
    static Color GREY;
    static Color WHITE;
};
