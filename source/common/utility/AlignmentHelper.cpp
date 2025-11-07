#include "AlignmentHelper.hpp"

using namespace mce;

unsigned int AlignmentHelper::getAlignedOffset(unsigned int size, unsigned int a2)
{
    unsigned int v2; // r3
    bool v3; // cf

    v2 = 16 - (size & 0xF);
    v3 = v2 >= 0x10;
    if ( v2 != 16 )
        v3 = v2 >= a2;
    if ( !v3 )
        size += v2;
    return size;
}