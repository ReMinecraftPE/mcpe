#pragma once

#include <stdint.h>
#include "common/Mth.hpp"

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
    void _init(float r, float g, float b, float a = 1.0f)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    void _init(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
    {
        this->r = float(r) / 255.0f;
        this->g = float(g) / 255.0f;
        this->b = float(b) / 255.0f;
        this->a = float(a) / 255.0f;
    }

    void _init(uint8_t r, uint8_t g, uint8_t b, float a)
    {
        _init(r, g, b);
        this->a = a;
    }

public:
    Color()
    {
        _init(0.0f, 0.0f, 0.0f, 0.0f);
    }

    Color(float r, float g, float b, float a = 1.0f)
    {
        _init(r, g, b, a);
    }

    Color(int r, int g, int b, int a = 255)
    {
        _init((uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a);
    }

    Color(int r, int g, int b, float a)
    {
        _init((uint8_t)r, (uint8_t)g, (uint8_t)b, a);
    }

    Color(unsigned int c)
    {
        _init(GET_RED(c), GET_GREEN(c), GET_BLUE(c), GET_ALPHA(c));
    }

    Color(unsigned int c, float alpha)
    {
        _init(GET_RED(c), GET_GREEN(c), GET_BLUE(c), alpha);
    }

    void fromHSB(float h, float s, float b);

    /*Color& blend(const Color& other)
    {
        r = Mth::Lerp(r, other.r, other.a);
        g = Mth::Lerp(g, other.g, other.a);
        b = Mth::Lerp(b, other.b, other.a);

        return *this;
    }*/

    Color& mulRGB(float mul)
    {
        r *= mul;
        g *= mul;
        b *= mul;
        return *this;
    }

    uint32_t toUInt32() const
    {
        uint8_t r8 = static_cast<uint8_t>(Mth::clamp(r * 255.0f, 0.0f, 255.0f));
        uint8_t g8 = static_cast<uint8_t>(Mth::clamp(g * 255.0f, 0.0f, 255.0f));
        uint8_t b8 = static_cast<uint8_t>(Mth::clamp(b * 255.0f, 0.0f, 255.0f));
        uint8_t a8 = static_cast<uint8_t>(Mth::clamp(a * 255.0f, 0.0f, 255.0f));

        return (r8 << 24) + (g8 << 16) + (b8 << 8) + a8;
    }

    Color operator+(float f) const
    {
        return Color(r + f, g + f, b + f, a);
    }

    Color operator+(const Color& c) const
    {
        return Color(r + c.r, g + c.g, b + c.b, a + c.a);
    }

    Color operator-(float f) const
    {
        return Color(r - f, g - f, b - f, a);
    }

    Color operator-(const Color& c) const
    {
        return Color(r - c.r, g - c.g, b - c.b, a - c.a);
    }

    Color operator*(float f) const
    {
        return Color(r * f, g * f, b * f, a);
    }

    Color operator*(const Color& c) const
    {
        return Color(r * c.r, g * c.g, b * c.b, a * c.a);
    }

    Color operator/(float f) const
    {
        return Color(r / f, g / f, b / f, a);
    }

    Color operator/(const Color& c) const
    {
        return Color(r / c.r, g / c.g, b / c.b, a / c.a);
    }

    Color& operator=(float rgb)
    {
        r = rgb;
        g = rgb;
        b = rgb;
        
        return *this;
    }

    Color& operator+=(const Color& c)
    {
        r += c.r;
        g += c.g;
        b += c.b;
        a += c.a;

        return *this;
    }

    Color& operator+=(float f)
    {
        r += f;
        g += f;
        b += f;

        return *this;
    }

    Color& operator-=(const Color& c)
    {
        r -= c.r;
        g -= c.g;
        b -= c.b;
        a -= c.a;

        return *this;
    }

    Color& operator-=(float f)
    {
        r -= f;
        g -= f;
        b -= f;

        return *this;
    }

    Color& operator*=(const Color& c)
    {
        r *= c.r;
        g *= c.g;
        b *= c.b;
        a *= c.a;

        return *this;
    }

    Color& operator*=(float f)
    {
        r *= f;
        g *= f;
        b *= f;

        return *this;
    }

    Color& operator/=(const Color& c)
    {
        r /= c.r;
        g /= c.g;
        b /= c.b;
        a /= c.a;

        return *this;
    }

    Color& operator/=(float f)
    {
        r /= f;
        g /= f;
        b /= f;

        return *this;
    }

    bool operator==(const Color& other) const
    {
        return this->r == other.r
            && this->g == other.g
            && this->b == other.b
            && this->a == other.a;
    }

    bool operator!=(const Color& other) const
    {
        return this->r != other.r
            || this->g != other.g
            || this->b != other.b
            || this->a != other.a;
    }

public:
    static Color FromRGB(uint8_t r, uint8_t g, uint8_t b)
    {
        return Color(r, g, b);
    }

    static Color FromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        return Color(r, g, b, a);
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

    static Color TEXT_GREY;
};