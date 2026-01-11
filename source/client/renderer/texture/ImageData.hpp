#pragma once

#include <string>
#include <stdint.h>
#include "ColorSpace.hpp"

struct ImageData
{
    int m_width;
    int m_height;
    uint8_t* m_data;
    unsigned int m_mipCount;
    ColorSpace m_colorSpace;

private:
    void _init();

public:
    ImageData();
    //ImageData(ImageData& other);
    ~ImageData();

    void release();
    void move(ImageData& other);
    void forceRGBA();
    bool isEmpty() const;
};
