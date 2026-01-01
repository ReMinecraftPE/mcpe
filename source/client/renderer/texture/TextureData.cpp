#include "TextureData.hpp"
#include "common/Util.hpp"
#include "renderer/RenderContextImmediate.hpp"

TextureData* TextureData::lastBoundTextureUnit;
TextureData* TextureData::lastBoundTexture[8];

void TextureData::_init()
{
    m_bEnableFiltering = false;
    m_bWrap = false;
    m_bDynamic = false;
}

void TextureData::_init(TextureData& other)
{
    _init();
    _move(other);
}

TextureData::TextureData()
{
    _init();
}

TextureData::TextureData(unsigned int width, unsigned int height, bool enableFiltering)
{
    _init();

    m_imageData.m_width = width;
    m_imageData.m_height = height;
    m_imageData.m_colorSpace = COLOR_SPACE_RGBA;
    //m_imageData.m_data.resize(4 * height * width);

    m_bEnableFiltering = enableFiltering;
}

TextureData::~TextureData()
{
    unload();
}

void TextureData::_move(TextureData& other)
{
    std::swap(this->m_bEnableFiltering, other.m_bEnableFiltering);
    std::swap(this->m_bWrap, other.m_bWrap);
    std::swap(this->m_bDynamic, other.m_bDynamic);
    this->m_imageData.move(other.m_imageData);
    this->m_texture.move(other.m_texture);
}

void TextureData::_loadTexData(mce::Texture& texture, bool enableFiltering, bool wrap, bool dynamic)
{
    mce::TextureDescription desc;
    desc.filteringLevel = enableFiltering ? mce::TEXTURE_FILTERING_BILINEAR : mce::TEXTURE_FILTERING_POINT;
    desc.bWrap = wrap;
    desc.width = m_imageData.m_width;
    desc.height = m_imageData.m_height;
    desc.bDynamic = dynamic;

    m_imageData.forceRGBA();
    
    switch (m_imageData.m_colorSpace)
    {
    case COLOR_SPACE_RGBA:
        desc.textureFormat = mce::TEXTURE_FORMAT_R8G8B8A8_UNORM;
        break;
    default:
        LOG_E("Failed to load texture for image with unsupported ColorSpace: %d", m_imageData.m_colorSpace);
        throw std::bad_cast();
    }

    if (!texture.isLoaded())
    {
        // texture bind #1
        texture.createTexture(mce::RenderContextImmediate::get(), desc);
    }

    // texture bind #2
    texture.subBuffer(mce::RenderContextImmediate::get(), m_imageData.m_data);
}

void TextureData::_loadMipmap(ImageData& data)
{
    if (data.m_colorSpace == COLOR_SPACE_RGB)
    {
        LOG_E("Functionality for mips and non RGBA textures isn't supported yet");
        throw std::bad_cast();
    }

    m_texture.convertToMipmapedTexture(mce::RenderContextImmediate::get(), 5);
    m_texture.loadMipMap(mce::RenderContextImmediate::get(), data.m_data, data.m_mipCount);
}

void TextureData::move(TextureData& other)
{
    unload();
    _move(other);
}

void TextureData::unload()
{
    if (m_texture.isLoaded())
    {
        m_texture.deleteTexture();
    }
}

void TextureData::clear()
{
    unload();
    m_imageData.release();
}

void TextureData::bind(unsigned int textureUnit)
{
    if (m_texture.isLoaded() && lastBoundTexture[textureUnit] != this)
    {
        lastBoundTexture[textureUnit] = this;
        m_texture.bindTexture(mce::RenderContextImmediate::get(), textureUnit);
    }
}

void TextureData::sync()
{
    if (m_texture.isLoaded())
    {
        _loadTexData(m_texture, m_bEnableFiltering, m_bWrap, m_bDynamic);
    }
}

uint32_t* TextureData::getData()
{
    return (uint32_t*)m_imageData.m_data;
}

void TextureData::setData(uint8_t* data)
{
    m_imageData.release();
    m_imageData.m_data = data;
    sync();
}

void TextureData::load()
{
    _loadTexData(m_texture, m_bEnableFiltering, m_bWrap, m_bDynamic);
    for (int i = 0; i < m_mipmaps.size(); i++)
    {
        _loadMipmap(m_mipmaps[i]);
    }
}

void TextureData::loadMipmap(ImageData& data)
{
    _loadMipmap(data);
    m_mipmaps.push_back(data);
}

bool TextureData::isEmpty() const
{
    return m_imageData.isEmpty();
}

void TextureData::unbind(unsigned int textureUnit)
{
    lastBoundTexture[textureUnit] = nullptr;
}

void TextureData::unbindAll()
{
    lastBoundTexture[0] = nullptr;
    lastBoundTexture[1] = nullptr;
    lastBoundTexture[2] = nullptr;
    lastBoundTexture[3] = nullptr;
    lastBoundTexture[4] = nullptr;
    lastBoundTexture[5] = nullptr;
    lastBoundTexture[6] = nullptr;
    lastBoundTexture[7] = nullptr;
}