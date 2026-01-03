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

void main( in PS_Input PSInput, out PS_Output PSOutput )
{
    PSOutput.color = float4( PSInput.uv.xy, 0, 1 );
}