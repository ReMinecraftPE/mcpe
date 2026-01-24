#pragma once

namespace mce
{
    enum VertexFieldType
    {
        VERTEX_FIELD_TYPE_UNKNOWN,
        //VERTEX_FIELD_TYPE_SINT8,     // @NOTE: Not natively supported on: D3D9
        //VERTEX_FIELD_TYPE_SINT8_4,   // 4D signed byte (@NOTE: Not natively supported on: D3D9)
        VERTEX_FIELD_TYPE_SINT8_4_N,   // Each of 4 bytes is normalized by dividing to 127.0 (@NOTE: Not natively supported on: D3D9)
        //VERTEX_FIELD_TYPE_UINT8,     // @NOTE: Not natively supported on: D3D9
        VERTEX_FIELD_TYPE_UINT8_4,     // 4D unsigned byte
        VERTEX_FIELD_TYPE_UINT8_4_N,   // Each of 4 bytes is normalized by dividing to 255.0
        //VERTEX_FIELD_TYPE_SINT16,    // @NOTE: Not natively supported on: D3D9
        VERTEX_FIELD_TYPE_SINT16_2,    // 2D signed short expanded to (value, value, 0., 1.)
        VERTEX_FIELD_TYPE_SINT16_2_N,  // 2D signed short normalized (v[0]/32767.0,v[1]/32767.0,0,1)
        VERTEX_FIELD_TYPE_SINT16_4,    // 4D signed short
        VERTEX_FIELD_TYPE_SINT16_4_N,  // 4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
        //VERTEX_FIELD_TYPE_UINT16,    // @NOTE: Not natively supported on: D3D9
        //VERTEX_FIELD_TYPE_UINT16_2,  // @NOTE: Not natively supported on: D3D9
        VERTEX_FIELD_TYPE_UINT16_2_N,  // 2D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,0,1)
        VERTEX_FIELD_TYPE_UINT16_4_N,  // 4D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
        //VERTEX_FIELD_TYPE_SINT32,    // @NOTE: Not natively supported on: D3D9
        //VERTEX_FIELD_TYPE_UINT32,    // @NOTE: Not natively supported on: D3D9
        //VERTEX_FIELD_TYPE_FLOAT16,   // @NOTE: Not natively supported on: D3D9
        //VERTEX_FIELD_TYPE_FLOAT16_2, // Two 16-bit floating point values, expanded to (value, value, 0, 1)
        //VERTEX_FIELD_TYPE_FLOAT16_4, // Four 16-bit floating point values
        VERTEX_FIELD_TYPE_FLOAT32,     // 1D float expanded to (value, 0., 0., 1.)
        VERTEX_FIELD_TYPE_FLOAT32_2,   // 2D float expanded to (value, value, 0., 1.)
        VERTEX_FIELD_TYPE_FLOAT32_3,   // 3D float expanded to (value, value, value, 1.)
        VERTEX_FIELD_TYPE_FLOAT32_4,   // 4D float
        VERTEX_FIELD_TYPES_COUNT
    };
}
