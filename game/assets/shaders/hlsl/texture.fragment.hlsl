#include "ShaderConstants.fxh"


struct PS_Input
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD_0;
};

struct PS_Output
{
    float4 color : SV_Target;
	
#ifndef _DIRECT3D9
    float depth  : SV_Depth; // Add this
#endif
};

PS_MAIN_BEGIN
    PSOutput.color = TEXTURE_0.Sample( TextureSampler0, PSInput.uv );

#ifdef ALPHA_TEST
    if( PSOutput.color.a < 0.5 )
    {
        discard;
    }
#endif

#ifndef _DIRECT3D9
	PSOutput.depth = 0.5f;
#endif
PS_MAIN_END
