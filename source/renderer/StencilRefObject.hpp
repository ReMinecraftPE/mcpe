#pragma once

#include <stdint.h>

namespace mce
{
    class StencilRefObject
    {
    private:
        uint8_t stencilRef;

    public:
        StencilRefObject(uint8_t ref = 0);

        uint8_t operator=(uint8_t value)
        {
            return stencilRef = value;
        }

        bool operator==(const StencilRefObject& other) const;
        bool operator!=(const StencilRefObject& other) const;

        operator uint8_t() const { return stencilRef; }
    };
}
