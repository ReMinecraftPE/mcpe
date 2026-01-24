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
    float4 diffuse = sampleTex0( TextureSampler0, PSInput.uv );


    if( diffuse.a < 0.5 )
    {
        discard;
    }

    PSOutput.color = CURRENT_COLOR;
PS_MAIN_END
