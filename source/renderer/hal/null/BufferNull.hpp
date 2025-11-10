#pragma once

#include <typeinfo>
#include "renderer/hal/base/BufferBase.hpp"

namespace mce
{
    class BufferNull : public BufferBase
    {
    private:
        uint64_t m_dataSize;

    public:
        BufferNull();
    };
}