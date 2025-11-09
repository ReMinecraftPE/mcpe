#pragma once

#include <vector>
#include "renderer/hal/interface/Texture.hpp"
#include "ImageData.hpp"

class TextureData
{
private:
    static TextureData* lastBoundTextureUnit;
    static TextureData* lastBoundTexture[8];

public:
    ImageData m_imageData;
    bool m_bEnableFiltering;
    bool m_bWrap;
    mce::Texture m_texture;
    std::vector<ImageData> m_mipmaps;

private:
    void _init();

public:
    TextureData();
    TextureData(TextureData& other);
    TextureData(unsigned int width, unsigned int height, bool enableFiltering);
    ~TextureData();

private:
    void _move(TextureData& other);
    void _loadTexData(mce::Texture& texture, bool enableFiltering, bool wrap);
    void _loadMipmap(ImageData& data);

public:
    void move(TextureData& other);
    void unload();
    void clear();
    void bind(unsigned int textureUnit);
    void sync();
    uint32_t* getData();
    void setData(uint8_t* data);
    void load();
    void loadMipmap(ImageData& data);
    bool isEmpty() const;

public:
    void operator=(TextureData& other);

public:
    static void unbind(unsigned int textureUnit);
    static void unbindAll();
};