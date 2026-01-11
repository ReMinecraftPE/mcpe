#include "VertexField.hpp"
#include "renderer/PlatformDefinitions.h"

using namespace mce;

const char* mce::vertexFieldNames[] = {
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