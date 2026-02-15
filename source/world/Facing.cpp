#include "Facing.hpp"

const Facing::Name Facing::OPPOSITE[6] =
{
    Facing::UP,    // DOWN -> UP
    Facing::DOWN,  // UP -> DOWN
    Facing::SOUTH, // NORTH -> SOUTH
    Facing::NORTH, // SOUTH -> NORTH
    Facing::EAST,  // WEST -> EAST
    Facing::WEST   // EAST -> WEST
};

const Facing::Name Facing::HORIZONTAL[4] =
{
    Facing::NORTH,
    Facing::SOUTH,
    Facing::EAST,
    Facing::WEST
};