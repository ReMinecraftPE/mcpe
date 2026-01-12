#include "Texture.hpp"

using namespace mce;

Texture::Texture()
    : MCE_GFX_CLASS(Texture)()
{
}

void Texture::loadMipMap(RenderContext& context, const void* rawTextureData, unsigned int mipMapLevel)
{
    const TextureDescription& description = getDescription();
    createMipMap(
        context,
        rawTextureData,
        description.width >> mipMapLevel,
        description.height >> mipMapLevel,
        mipMapLevel
    );
}
