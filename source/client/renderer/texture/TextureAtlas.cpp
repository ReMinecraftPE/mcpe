#include <cstring>
#include <algorithm>
#include "TextureAtlas.hpp"
#include "common/Utils.hpp"

TextureAtlas::TextureAtlas(const std::string& name, bool enableFiltering) :
    m_currentX(0),
    m_currentY(0),
    m_shelfHeight(0),
    m_name(name),
    m_texture(DEFAULT_ATLAS_SIZE, DEFAULT_ATLAS_SIZE, enableFiltering)
{
    _init();
}

TextureAtlas::TextureAtlas(const std::string& name, int initialSize, bool enableFiltering) :
    m_currentX(0),
    m_currentY(0),
    m_shelfHeight(0),
    m_name(name),
    m_texture(initialSize, initialSize, enableFiltering)
{
    _init();
}

void TextureAtlas::addSprite(const std::string& name, uint8_t* data, int width, int height)
{
    PendingSprite sprite;
    sprite.name = name;
    sprite.data = data;
    sprite.width = width;
    sprite.height = height;
    m_pendingSprites.push_back(sprite);
}

void TextureAtlas::addSprite(const std::string& name, const TextureData& texture)
{
    if (texture.isEmpty()) return;

    int size = texture.m_imageData.m_width * texture.m_imageData.m_height * 4;
    uint8_t* data = new uint8_t[size];
    memcpy(data, texture.m_imageData.m_data, size);
    addSprite(name, data, texture.m_imageData.m_width, texture.m_imageData.m_height);
}

class SpriteSorter
{
public:
    bool operator()(const PendingSprite& a, const PendingSprite& b)
    {
        return a.height > b.height;
    }
};

bool TextureAtlas::build()
{
    std::sort(m_pendingSprites.begin(), m_pendingSprites.end(), SpriteSorter());

    while (!pack())
    {
        if (!grow())
            return false;
    }

    for (size_t i = 0; i < m_pendingSprites.size(); ++i)
    {
        delete[] m_pendingSprites[i].data;
    }

    m_pendingSprites.clear();
    return true;
}

void TextureAtlas::_init()
{
    m_texture.m_imageData.release();

    int size = getWidth() * getHeight() * 4;
    uint8_t *mem = (uint8_t *)malloc(size);
    if (!mem)
        throw std::bad_alloc();
    m_texture.m_imageData.m_data = mem;
    memset(m_texture.m_imageData.m_data, 0, size);
}

bool TextureAtlas::pack()
{
    m_currentX = 0;
    m_currentY = 0;
    m_shelfHeight = 0;
    m_sprites.clear();

    for (size_t i = 0; i < m_pendingSprites.size(); ++i)
    {
        const PendingSprite& pending = m_pendingSprites[i];

        if (pending.width > getWidth())
            return false;

        if (m_currentX + pending.width > getWidth())
        {
            m_currentX = 0;
            m_currentY += m_shelfHeight;
            m_shelfHeight = 0;
        }

        if (m_currentY + pending.height > getHeight())
            return false;

        blitSprite(pending.data, pending.width, pending.height, m_currentX, m_currentY);

        m_sprites[pending.name] = TextureAtlasSprite(
            pending.name,
            m_currentX, m_currentY,
            pending.width, pending.height,
            this
        );

        m_currentX += pending.width;
        if (pending.height > m_shelfHeight)
            m_shelfHeight = pending.height;
    }

    return true;
}

void TextureAtlas::blitSprite(uint8_t* src, int srcWidth, int srcHeight, int destX, int destY)
{
    for (int y = 0; y < srcHeight; ++y)
    {
        memcpy(
            m_texture.m_imageData.m_data + ((destY + y) * getWidth() + destX) * 4,
            src + (y * srcWidth) * 4,
            srcWidth * 4
        );
    }
}

bool TextureAtlas::grow()
{
    int newWidth = getWidth() + DEFAULT_ATLAS_SIZE;

    if (newWidth > MAX_ATLAS_SIZE)
        return false;

    m_texture.m_imageData.m_width = newWidth;

    _init();
    return true;
}

const TextureAtlasSprite* TextureAtlas::getSprite(const std::string& name) const
{
    std::map<std::string, TextureAtlasSprite>::const_iterator it = m_sprites.find(name);
    if (it != m_sprites.end())
        return &it->second;
    return nullptr;
}

int TextureAtlas::getWidth() const
{
    return m_texture.m_imageData.m_width;
}

int TextureAtlas::getHeight() const
{
    return m_texture.m_imageData.m_height;
}

TextureAtlasSprite::TextureAtlasSprite(const std::string& name, int x, int y, int width, int height, TextureAtlas* atlas) :
    IntRectangle(x, y, width, height),
    name(name),
    m_pAtlas(atlas)
{
    minU = float(x) / atlas->getWidth();
    minV = float(y) / atlas->getHeight();
    maxU = float(x + width) / atlas->getWidth();
    maxV = float(y + height) / atlas->getHeight();
}
