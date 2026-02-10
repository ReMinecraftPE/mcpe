#pragma once

#include "IntRectangle.hpp"

class AreaNavigation
{
public:
    typedef int ID;

    enum Direction
    {
        UP,
        DOWN,
        LEFT,
        RIGHT
    };

    ID navigate(Direction dir, int x, int y, bool cycle = false);

    ID navigateCyclic(Direction dir, int x, int y);

    virtual bool next(int& x, int& y, bool cycle) = 0;

    virtual bool isValid(ID);

protected:
    ID m_index;
    IntRectangle m_area;
};