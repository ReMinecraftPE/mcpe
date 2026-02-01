#pragma once
#include <vector>
#include <string>
#include <map>
#include <cstdint>
#include "TextureData.hpp"

#define DEFAULT_ATLAS_SIZE (256)
#define MAX_ATLAS_SIZE (16384)

class TextureAtlas;

struct TextureAtlasSprite
{
    std::string name;
    int x;
    int y;
    int width;
    int height;
    float minU, minV, maxU, maxV;
    TextureAtlas* m_pAtlas;

    TextureAtlasSprite() :
        x(0),
        y(0),
        width(0),
        height(0),
        minU(0.0f),
        minV(0.0f),
        maxU(0.0f),
        maxV(0.0f),
        m_pAtlas(nullptr)
    {
    }

    TextureAtlasSprite(const std::string& name, int x, int y, int width, int height, TextureAtlas* atlas);
};

struct PendingSprite
{
    std::string name;
    uint8_t* data;
    int width;
    int height;
};

class TextureAtlas
{
public:
    TextureAtlas(const std::string& name, bool enableFiltering = false);
    TextureAtlas(const std::string& name, int initialSize, bool enableFiltering = false);

    void addSprite(const std::string& name, uint8_t* data, int width, int height);
    void addSprite(const std::string& name, const TextureData&);

    bool build();

    const TextureAtlasSprite* getSprite(const std::string& name) const;

    int getWidth() const;

    int getHeight() const;

private:
    void _init();

    bool pack();

    void blitSprite(uint8_t* src, int srcWidth, int srcHeight, int destX, int destY);

    bool grow();


private:
    std::vector<PendingSprite> m_pendingSprites;
    std::map<std::string, TextureAtlasSprite> m_sprites;

    int m_currentX;
    int m_currentY;
    int m_shelfHeight;

public:
    std::string m_name;
    TextureData m_texture;
};