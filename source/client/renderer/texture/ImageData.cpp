#include <typeinfo>
#include "ImageData.hpp"
#include "compat/LegacyCPP.hpp"
#include "common/Logger.hpp"

void ImageData::_init()
{
    m_width = 0;
    m_height = 0;
    m_data = nullptr;
    m_mipCount = 0;
    m_colorSpace = COLOR_SPACE_RGBA;
}

ImageData::ImageData()
{
    _init();
}

/*ImageData::ImageData(ImageData& other)
{
    _init();
    move(other);
}*/

ImageData::~ImageData()
{
    release();
}

void ImageData::release()
{
    if (m_data)
    {
        delete[] m_data;
        m_data = nullptr;
    }
}

void ImageData::move(ImageData& other)
{
    this->m_height = other.m_height;
    this->m_mipCount = other.m_mipCount;
    this->m_width = other.m_width;
    uint8_t* tempData = this->m_data;
    this->m_data = other.m_data;
    other.m_data = tempData;
    this->m_colorSpace = other.m_colorSpace;
    this->m_mipCount = other.m_mipCount;
}

void ImageData::forceRGBA()
{
    if (m_colorSpace == COLOR_SPACE_RGBA || m_data == nullptr)
        return;

    if (m_colorSpace != COLOR_SPACE_RGB)
    {
        LOG_E("Can't convert non-RGB image to RGBA");
        throw std::bad_cast();
    }

    size_t size = m_height * m_width;
    uint8_t* rgbData = m_data;
    uint8_t* rgbaData = new uint8_t[4 * size];

    uint8_t* rgbDataItr = rgbData;
    uint8_t* rgbaDataItr = rgbaData;

    // translate RGB buffer into RGBA buffer
    uint8_t* rgbDataEnd = &rgbData[3 * size];
    while (rgbDataItr != rgbDataEnd)
    {
        rgbaDataItr[0] = rgbDataItr[0]; // red
        rgbaDataItr[1] = rgbDataItr[1]; // green
        rgbaDataItr[2] = rgbDataItr[2]; // blue
        rgbaDataItr[3] = 255;           // alpha

        rgbDataItr += 3;
        rgbaDataItr += 4;
    }

    m_data = rgbaData;
    delete[] rgbData;
    m_colorSpace = COLOR_SPACE_RGBA;
}

bool ImageData::isEmpty() const
{
    return !m_data || m_width == 0 || m_height == 0;
}
