#include <typeinfo>

#include "TextureBase.hpp"
#include "common/Logger.hpp"

using namespace mce;

const TextureDescription& TextureBase::getDescription() const
{
    return m_description;
}

void TextureBase::deleteTexture()
{
    m_bCreated = false;
}

void TextureBase::bindTexture(RenderContext& context, unsigned int textureUnit, unsigned int shaderStagesBits)
{
}

bool TextureBase::isLoaded() const
{
    return m_bCreated;
}

void TextureBase::convertToMipmapedTexture(unsigned int mipmaps)
{
    if (m_description.mipCount == mipmaps)
        return;
    
    if (m_description.filteringLevel == TEXTURE_FILTERING_BILINEAR)
    {
        LOG_E("Unsupported filtering level for mip maps, please add the correct filtering case: %d", m_description.filteringLevel);
        throw std::bad_cast();
    }

    m_description.filteringLevel = TEXTURE_FILTERING_MIPMAP_BILINEAR;
}

void TextureBase::convertToMipmapedTexture(RenderContext& context, unsigned int mipmaps)
{
}

void TextureBase::subBuffer(RenderContext& context, const void* pixels, unsigned int xoffset, unsigned int yoffset, unsigned int width, unsigned int height, unsigned int level)
{
}

void TextureBase::subBuffer(RenderContext& context, const void* pixels)
{
}

void TextureBase::copyTexture(RenderContext& context, Texture* src, unsigned int startX, unsigned int startY, unsigned int width, unsigned int height)
{
}

void TextureBase::createMipMap(RenderContext& context, const void* pixels, unsigned int width, unsigned int height, unsigned int level)
{
}
        
void TextureBase::createTexture(const TextureDescription& description)
{
    m_description = description;
}

void TextureBase::createTexture(RenderContext& context, TextureDescription const&)
{
}

void TextureBase::lock(RenderContext& context)
{
}

void TextureBase::unlock(RenderContext& context)
{
}

bool TextureBase::supportsMipMaps()
{
    return false;
}
