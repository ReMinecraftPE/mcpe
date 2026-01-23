#include "VertexField.hpp"
#include "renderer/PlatformDefinitions.h"
#include "GameMods.hpp"

namespace mce
{
    const char* vertexFieldNames[] = {
        /* POSITION */ "POSITION",
        /* COLOR*/     "COLOR",
        /* NORMAL*/    "NORMAL",
#if MCE_GFX_API_D3D9
        /* UV0 */      "TEXCOORD0",
        /* UV1 */      "TEXCOORD1"
#else
        /* UV0 */      "TEXCOORD_0",
        /* UV1 */      "TEXCOORD_1"
#endif
    };

    const VertexFieldType vertexFieldTypes[] = {
        /* POSITION */ VERTEX_FIELD_TYPE_FLOAT32_3,
        /* COLOR*/     VERTEX_FIELD_TYPE_UINT8_4_N,
#if MCE_GFX_SUPPORTS_SINT8_4_N
        /* NORMAL*/    VERTEX_FIELD_TYPE_SINT8_4_N, // Everything else
#elif MCE_GFX_SUPPORTS_UINT8_4_N
        /* NORMAL*/    VERTEX_FIELD_TYPE_UINT8_4, // D3D9, should be normalized, but this somehow looks like it has no lighting
#else
#error Couldn't choose a VertexFieldType for NORMAL
#endif
#ifdef ENH_GFX_COMPACT_UVS
        // @NOTE: this assumes RenderContext::supports16BitUnsignedUVs() is true
        /* UV0 */      VERTEX_FIELD_TYPE_UINT16_2_N,
        /* UV1 */      VERTEX_FIELD_TYPE_UINT16_2_N
#else
        /* UV0 */      VERTEX_FIELD_TYPE_FLOAT32_2,
        /* UV1 */      VERTEX_FIELD_TYPE_FLOAT32_2
#endif
    };
}
