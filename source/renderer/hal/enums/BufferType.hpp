#pragma once

namespace mce
{
    enum BufferType
    {
        BUFFER_TYPE_VERTEX,
        BUFFER_TYPE_INDEX,

        BUFFER_TYPES_MIN = BUFFER_TYPE_VERTEX,
        BUFFER_TYPES_MAX = BUFFER_TYPE_INDEX,
        BUFFER_TYPE_NONE = 3
    };
}