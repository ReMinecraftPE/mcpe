#include "ShaderConstants.fxh"


struct PS_Input
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD_0;
};

struct PS_Output
{
    float4 color : SV_Target;
    float depth  : SV_Depth; // Add this
};

PS_MAIN_BEGIN
    PSOutput.color = sampleTex0( TextureSampler0, PSInput.uv );

#ifdef ALPHA_TEST
    if( PSOutput.color.a < 0.5 )
    {
        discard;
    }
#endif

	PSOutput.depth = 0.5f;
PS_MAIN_END
