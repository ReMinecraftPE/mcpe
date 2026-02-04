#pragma once

class Tool
{
public:
    enum Type
    {
        NONE    = 0,
        PICKAXE = 1 << 0,
        SHOVEL  = 1 << 1,
        HATCHET = 1 << 2,
        SWORD   = 1 << 3,
        SHEARS  = 1 << 4
    };
};