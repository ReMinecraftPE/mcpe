#include "AlignmentHelper.hpp"

using namespace mce;

unsigned int AlignmentHelper::getAlignedOffset(unsigned int size, unsigned int a2)
{
    unsigned int v2;

    v2 = 16 - (size & 0xF);
    if (v2 != 16 && v2 < a2)
        size += v2;

    return size;
}