#include <typeinfo>

#include "renderer/hal/helpers/ErrorHandler.hpp"
#include "API_OGL.hpp"
#include "TextureOGL.hpp"

using namespace mce;

TextureOGL::TextureOGL()
    : TextureBase()
{
}

GLenum getOpenGLTextureFormat(TextureFormat textureFormat)
{
    switch (textureFormat)
    {
        case TEXTURE_FORMAT_R8G8B8A8_UNORM:
            return GL_RGBA;
        default:
            LOG_E("Unknown textureFormat: %d", textureFormat);
            throw std::bad_cast();
    }
}

GLint getOpenGLInternalTextureFormatFromTextureFormat(TextureFormat textureFormat)
{
    switch (textureFormat)
    {
        case TEXTURE_FORMAT_R8G8B8A8_UNORM:
            return GL_RGBA8;
        default:
            LOG_E("Unknown textureFormat: %d", textureFormat);
            throw std::bad_cast();
    }
}

GLenum getOpenGLTextureTypeFromTextureFormat(TextureFormat textureFormat)
{
    switch (textureFormat)
    {
        case TEXTURE_FORMAT_R8G8B8A8_UNORM:
            return GL_UNSIGNED_BYTE;
        default:
            LOG_E("Unknown textureFormat: %d", textureFormat);
            throw std::bad_cast();
    }
}

void TextureOGL::deleteTexture()
{
    glDeleteTextures(1, &m_state.m_textureName);
    TextureBase::deleteTexture();

    *this = TextureOGL();

    ErrorHandler::checkForErrors();
}

void TextureOGL::bindTexture(RenderContext& context, unsigned int textureUnit, unsigned int shaderStagesBits)
{
    ErrorHandler::checkForErrors();

    GLenum texture = GL_TEXTURE0 + textureUnit;
    if (context.m_activeTexture != texture)
    {
        xglActiveTexture(texture);
        context.m_activeTexture = texture;
    }

    ErrorHandler::checkForErrors();

#ifdef MC_GL_DEBUG
    LOG_I("glBindTexture: target=0x%X, texture=%d", m_state.m_textureTarget, m_state.m_textureName);
#endif
    glBindTexture(m_state.m_textureTarget, m_state.m_textureName);

    RenderContextOGL::ActiveTextureUnit& activeTextureUnit = context.getActiveTextureUnit(textureUnit);
    activeTextureUnit.m_textureUnit = textureUnit;
    activeTextureUnit.m_bIsShaderUniformDirty = true;

    ErrorHandler::checkForErrors();
}

void TextureOGL::convertToMipmapedTexture(RenderContext& context, unsigned int mipmaps)
{
    if (mipmaps == 0)
    {
        LOG_E("You must have a positive number of mip maps that is greater than 1");
        throw std::bad_cast();
    }

    bindTexture(context);
    TextureBase::convertToMipmapedTexture(mipmaps - 1);

    glTexParameteri(m_state.m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
     // @NOTE: Need GL 1.2 for GL_TEXTURE_MAX_LEVEL
    glTexParameteri(m_state.m_textureTarget, GL_TEXTURE_MAX_LEVEL, mipmaps - 1);

    ErrorHandler::checkForErrors();
}

void TextureOGL::subBuffer(RenderContext& context, const void* pixels, unsigned int xoffset, unsigned int yoffset, unsigned int width, unsigned int height, unsigned int level)
{
    bindTexture(context);
    ErrorHandler::checkForErrors();

    if (m_state.m_textureTarget != GL_TEXTURE_2D)
    {
        LOG_E("Unknown textureTarget %d", m_state.m_textureTarget);
        throw std::bad_cast();
    }

#ifdef MC_GL_DEBUG
    LOG_I("glTexSubImage2D: xoffset=%d, yoffset=%d, width=%d, height=%d, format=0x%X, type=0x%X, pixels=0x%X", xoffset, yoffset, width, height, m_state.m_textureFormat, m_state.m_textureType, pixels);
#endif
    glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, m_state.m_textureFormat, m_state.m_textureType, pixels);
    ErrorHandler::checkForErrors();
}

void TextureOGL::subBuffer(RenderContext& context, const void* pixels)
{
    subBuffer(context, pixels, 0, 0, m_description.width, m_description.height, 0);
}

void TextureOGL::createMipMap(RenderContext& context, const void* pixels, unsigned int width, unsigned int height, unsigned int level)
{
    if (m_state.m_textureTarget != GL_TEXTURE_2D)
    {
        LOG_E("Unknown textureTarget %d", m_state.m_textureTarget);
        throw std::bad_cast();
    }
    
#ifdef MC_GL_DEBUG
    LOG_I("glTexImage2D: internalFormat=0x%X, width=%d, height=%d, format=0x%X, type=0x%X, pixels=0x%X", m_state.m_internalTextureFormat, width, height, m_state.m_textureFormat, m_state.m_textureType, pixels);
#endif
    glTexImage2D(GL_TEXTURE_2D, level, m_state.m_internalTextureFormat, width, height, 0, m_state.m_textureFormat, m_state.m_textureType, pixels);
    m_bCreated = true;
    ErrorHandler::checkForErrors();
}

void TextureOGL::createTexture(RenderContext& context, const TextureDescription& description)
{
    TextureBase::createTexture(description);
    glGenTextures(1, &m_state.m_textureName);
    ErrorHandler::checkForErrors();
    
    m_state.m_internalTextureFormat = getOpenGLInternalTextureFormatFromTextureFormat(description.textureFormat);
    m_state.m_textureFormat = getOpenGLTextureFormat(description.textureFormat);
    m_state.m_textureType = getOpenGLTextureTypeFromTextureFormat(description.textureFormat);

    bindTexture(context);
    ErrorHandler::checkForErrors();
    createMipMap(context, nullptr, description.width, description.height, 0);

    switch (description.filteringLevel)
    {
    case TEXTURE_FILTERING_BILINEAR:
        // @NOTE: Need GL 1.2 for GL_CLAMP_TO_EDGE
        glTexParameteri(m_state.m_textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(m_state.m_textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(m_state.m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(m_state.m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    case TEXTURE_FILTERING_POINT:
        glTexParameteri(m_state.m_textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(m_state.m_textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(m_state.m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(m_state.m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    case TEXTURE_FILTERING_MIPMAP_BILINEAR:
        glTexParameteri(m_state.m_textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(m_state.m_textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(m_state.m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(m_state.m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    default:
        break;
    }

    ErrorHandler::checkForErrors();
}

void TextureOGL::lock(RenderContext& context)
{
    context.m_activePixels.resize(m_description.getSizeInBytes());
}

void TextureOGL::unlock(RenderContext& context)
{
    bindTexture(context);
    subBuffer(context, context.m_activePixels.data());
    context.m_activePixels.clear();
}

bool TextureOGL::supportsMipMaps()
{
    return gl::supportsMipmaps();
}