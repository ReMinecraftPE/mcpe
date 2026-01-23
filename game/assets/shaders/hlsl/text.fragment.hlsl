#include "ShaderConstants.fxh"


struct PS_Input
{
    float4 position : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD_0;
};

struct PS_Output
{
    float4 color : SV_Target;
};

PS_MAIN_BEGIN
    const float4 diffuse = sampleTex0( TextureSampler0, PSInput.uv );

#ifdef ALPHA_TEST
    if( diffuse.a < 0.5 )
    {
        discard;
    }
#endif

    PSOutput.color = diffuse * PSInput.color * DARKEN;
PS_MAIN_END
