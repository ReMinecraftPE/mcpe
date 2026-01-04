#include <typeinfo>

#include "TextureBase.hpp"
#include "common/Logger.hpp"
#include "renderer/PlatformDefinitions.h"

using namespace mce;

TextureBase::TextureBase()
{
	m_bCreated = false;
    m_bHasWriteAccess = false;
}

void TextureBase::_bindWriteBuffer(RenderContext& context)
{
    if (!m_description.bIsStaging)
    {
        LOG_E("bindWriteBuffer can only be called on staging Textures");
        throw std::bad_cast();
    }

    if (m_bHasWriteAccess)
    {
        LOG_W("bindWriteBuffer called when write buffer is already bound");
    }

    m_bHasWriteAccess = true;
}

void TextureBase::_releaseWriteBuffer(RenderContext& context)
{
    if (!m_description.bIsStaging)
    {
        LOG_E("releaseWriteBuffer can only be called on staging Textures");
        throw std::bad_cast();
    }

    if (!m_bHasWriteAccess)
    {
        LOG_W("releaseWriteBuffer called when write buffer was already released");
    }

    m_bHasWriteAccess = false;
}

const TextureDescription& TextureBase::getDescription() const
{
    return m_description;
}

void TextureBase::deleteTexture()
{
    if (m_bHasWriteAccess)
    {
        LOG_E("Tried to delete Texture that was left in write mode. Please call releaseWriteBuffer before deleting the Texture.");
        throw std::bad_cast();
    }

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

    m_description.mipCount = mipmaps;
    
    if (m_description.filteringLevel != TEXTURE_FILTERING_POINT)
    {
        LOG_E("Unsupported filtering level for mip maps, please add the correct filtering case: %d", m_description.filteringLevel);
        throw std::bad_cast();
    }

    m_description.filteringLevel = TEXTURE_FILTERING_MIPMAP_BILINEAR;
}

void TextureBase::convertToMipmapedTexture(RenderContext& context, unsigned int mipmaps)
{
}

void TextureBase::enableWriteMode(RenderContext& context)
{
    if (m_bHasWriteAccess)
    {
        LOG_W("enableWriteMode called when write mode was already enabled");
    }

    m_bHasWriteAccess = true;
}

void TextureBase::disableWriteMode(RenderContext& context)
{
    if (!m_bHasWriteAccess)
    {
        LOG_W("disableWriteMode called when write mode was already disabled");
    }

    m_bHasWriteAccess = false;
}

void TextureBase::subBuffer(RenderContext& context, const void* pixels, unsigned int xoffset, unsigned int yoffset, unsigned int width, unsigned int height, unsigned int level)
{
    /*if (!m_description.bDynamic)
    {
        // To OpenGL, all textures are dynamic, to Direct3D, this is not the case.
        LOG_E("Tried to write to static texture! Texture must be dynamic to be written to.");
        throw std::bad_cast();
    }*/

#if MCE_GFX_DYNAMIC_TEXTURE_STAGING
    if (!m_description.bIsStaging)
    {
        // To OpenGL, all textures on the GPU are modifiable, to Direct3D, this is not the case.
        LOG_E("Tried to write to a non-staging texture! Textures must be explicitly created for staging in order to be modified directly.");
        throw std::bad_cast();
    }
#endif
    
    if (!m_bHasWriteAccess)
    {
        LOG_E("Cannot write to buffer without write-access. Did you call bindWriteBuffer? Don't forget to release it after.");
        throw std::bad_cast();
    }
}

void TextureBase::subBuffer(RenderContext& context, const void* pixels)
{
}

void TextureBase::copyTexture(RenderContext& context, const Texture* src, unsigned int startX, unsigned int startY, unsigned int width, unsigned int height)
{
}

void TextureBase::createMipMap(RenderContext& context, const void* pixels, unsigned int width, unsigned int height, unsigned int level)
{
}
        
void TextureBase::createTexture(const TextureDescription& description)
{
    m_description = description;
}

void TextureBase::createTexture(RenderContext& context, const TextureDescription& description)
{
    createTexture(description);
    m_bCreated = true;
}

void TextureBase::lock(RenderContext& context)
{
}

void TextureBase::unlock(RenderContext& context)
{
}

void TextureBase::move(TextureBase& other)
{
    std::swap(this->m_description, other.m_description);
    std::swap(this->m_bCreated, other.m_bCreated);
    std::swap(this->m_bHasWriteAccess, other.m_bHasWriteAccess);
}

bool TextureBase::supportsMipMaps()
{
    return false;
}
