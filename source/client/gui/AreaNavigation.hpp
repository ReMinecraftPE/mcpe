#pragma once

#include "IntRectangle.hpp"

class AreaNavigation
{
public:
    typedef int ID;

public:
    enum Direction
    {
        UP,
        DOWN,
        LEFT,
        RIGHT
    };

public:
    ID navigate(Direction dir, int x, int y, bool cycle = false);
    ID navigateCyclic(Direction dir, int x, int y);

    virtual bool next(int& x, int& y, bool cycle) = 0;
    virtual bool isValid(ID);

protected:
    ID m_index;
    IntRectangle m_area;
};