#include "AlignmentHelper.hpp"

using namespace mce;

unsigned int AlignmentHelper::getAlignedOffset(unsigned int startOffset, unsigned int size)
{
    unsigned int v2;

    v2 = 16 - (startOffset & 0xF);
    if (v2 != 16 && v2 < size)
        startOffset += v2;

    return startOffset;
}
