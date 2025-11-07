#include "Texture.hpp"

using namespace mce;

Texture::Texture()
    : TextureOGL()
{
}

void Texture::bindTexture(RenderContext& context, unsigned int textureUnit, unsigned int shaderStagesBits)
{
    TextureOGL::bindTexture(context, textureUnit, shaderStagesBits);
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