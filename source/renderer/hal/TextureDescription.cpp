#include "TextureDescription.hpp"

using namespace mce;

TextureDescription::TextureDescription()
{
    sampleCount = 1;
    mipCount = 1;
    bWrap = false;
    filteringLevel = TEXTURE_FILTERING_POINT;
    bIsStaging = false;
}
