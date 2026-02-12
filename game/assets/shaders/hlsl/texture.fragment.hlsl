#include "ShaderConstants.fxh"


struct PS_Input
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD_0;
};

struct PS_Output
{
    float4 color : SV_Target;
};

PS_MAIN_BEGIN
    PSOutput.color = sampleTex0( TextureSampler0, PSInput.uv );

#ifdef ALPHA_TEST
    if( PSOutput.color.a < 0.5 )
    {
        discard;
    }
#endif

PS_MAIN_END
