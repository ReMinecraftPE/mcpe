#pragma once

#include "world/Container.hpp"

class Recipe
{
public:
    virtual ~Recipe() {}

    virtual bool matches(Container* container) = 0;
    virtual const ItemStack& assemble(Container* container) = 0;
    virtual int size() const = 0;
    virtual bool isShaped() const
    {
        return false;
    }
};