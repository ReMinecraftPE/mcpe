#pragma once

namespace mce
{
    class StencilRefObject
    {
    private:
        unsigned int stencilRef;

    public:
        StencilRefObject(unsigned int ref = 0);

        unsigned int operator=(unsigned int value)
        {
            return stencilRef = value;
        }

        bool operator==(const StencilRefObject& other) const;
        bool operator!=(const StencilRefObject& other) const;

        operator unsigned int() const { return stencilRef; }
    };
}