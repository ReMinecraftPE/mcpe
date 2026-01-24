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
    PSOutput.color = float4( PSInput.uv.xy, 0, 1 );
PS_MAIN_END
