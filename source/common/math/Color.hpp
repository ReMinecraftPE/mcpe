#pragma once

struct Color
{
public:
    float r;
    float g;
    float b;
    float a;

public:
    Color()
    {
		*this = NIL;
    }

    Color(float r, float g, float b, float a = 1.0f)
        : r(r), g(g), b(b), a(a)
    {
    }

    void fromHSB(float h, float s, float b);

    void operator=(const Color& other)
    {
        this->r = other.r;
        this->g = other.g;
        this->b = other.b;
        this->a = other.a;
    }

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
