#include <typeinfo>

#include "TextureOGL.hpp"
#include "API_OGL.hpp"
#include "helpers/ErrorHandlerOGL.hpp"

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
            return GL_RGBA;
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

void TextureOGL::_subBuffer(RenderContext& context, const void* pixels, unsigned int xoffset, unsigned int yoffset, unsigned int width, unsigned int height, unsigned int level)
{
    bindTexture(context);
    ErrorHandlerOGL::checkForErrors();

    if (m_state.m_textureTarget != GL_TEXTURE_2D)
    {
        LOG_E("Unknown textureTarget %d", m_state.m_textureTarget);
        throw std::bad_cast();
    }

    glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, m_state.m_textureFormat, m_state.m_textureType, pixels);
    ErrorHandlerOGL::checkForErrors();
}

void TextureOGL::deleteTexture()
{
	ErrorHandlerOGL::checkForErrors();

    glDeleteTextures(1, &m_state.m_textureName);
    TextureBase::deleteTexture();

    *this = TextureOGL();

    ErrorHandlerOGL::checkForErrors();
}

void TextureOGL::bindTexture(RenderContext& context, unsigned int textureUnit, unsigned int shaderStagesBits)
{
    ErrorHandlerOGL::checkForErrors();

    GLenum texture = GL_TEXTURE0 + textureUnit;
    if (context.m_activeTexture != texture)
    {
        xglActiveTexture(texture);
        context.m_activeTexture = texture;
    }

    ErrorHandlerOGL::checkForErrors();

    glBindTexture(m_state.m_textureTarget, m_state.m_textureName);

    RenderContextOGL::ActiveTextureUnit& activeTextureUnit = context.getActiveTextureUnit(textureUnit);
    activeTextureUnit.m_textureUnit = textureUnit;
    activeTextureUnit.m_bIsShaderUniformDirty = true;

    ErrorHandlerOGL::checkForErrors();
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
    if (gl::isOpenGLES3() || !gl::isOpenGLES())
    {
        glTexParameteri(m_state.m_textureTarget, GL_TEXTURE_MAX_LEVEL, mipmaps - 1);
    }

    ErrorHandlerOGL::checkForErrors();
}

void TextureOGL::subBuffer(RenderContext& context, const void* pixels, unsigned int xoffset, unsigned int yoffset, unsigned int width, unsigned int height, unsigned int level)
{
    TextureBase::subBuffer(context, pixels, xoffset, yoffset, width, height, level);
    _subBuffer(context, pixels, xoffset, yoffset, width, height, level);
}

void TextureOGL::subBuffer(RenderContext& context, const void* pixels)
{
    TextureBase::subBuffer(context, pixels);
    _subBuffer(context, pixels, 0, 0, m_description.width, m_description.height, 0);
}

void TextureOGL::createMipMap(RenderContext& context, const void* pixels, unsigned int width, unsigned int height, unsigned int level)
{
    if (m_state.m_textureTarget != GL_TEXTURE_2D)
    {
        LOG_E("Unknown textureTarget %d", m_state.m_textureTarget);
        throw std::bad_cast();
    }
    
    glTexImage2D(GL_TEXTURE_2D, level, m_state.m_internalTextureFormat, width, height, 0, m_state.m_textureFormat, m_state.m_textureType, pixels);
    m_bCreated = true;
    ErrorHandlerOGL::checkForErrors();
}

void TextureOGL::createTexture(RenderContext& context, const TextureDescription& description)
{
    TextureBase::createTexture(description);
    glGenTextures(1, &m_state.m_textureName);
    ErrorHandlerOGL::checkForErrors();
    
    m_state.m_internalTextureFormat = getOpenGLInternalTextureFormatFromTextureFormat(description.textureFormat);
    m_state.m_textureFormat = getOpenGLTextureFormat(description.textureFormat);
    m_state.m_textureType = getOpenGLTextureTypeFromTextureFormat(description.textureFormat);

    bindTexture(context);
    ErrorHandlerOGL::checkForErrors();
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
        if (description.bWrap)
        {
            glTexParameteri(m_state.m_textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(m_state.m_textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }
        else
        {
            glTexParameteri(m_state.m_textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(m_state.m_textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
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

    ErrorHandlerOGL::checkForErrors();
}

void TextureOGL::lock(RenderContext& context)
{
    context.m_activePixels.resize(m_description.getSizeInBytes());
}

void TextureOGL::unlock(RenderContext& context)
{
    bindTexture(context);
    subBuffer(context, &context.m_activePixels[0]);
    context.m_activePixels.clear();
}

void TextureOGL::move(TextureOGL& other)
{
    TextureBase::move(other);
    State tempState = this->m_state;
    this->m_state = other.m_state;
    other.m_state = tempState;
}

bool TextureOGL::supportsMipMaps()
{
    return gl::supportsMipmaps();
}