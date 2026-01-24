#include "ShaderConstants.fxh"

struct VS_Input
{
    float3 position : POSITION;
};


struct PS_Input
{
    float4 position : SV_Position;
};


VS_MAIN_BEGIN
    PSInput.position = mul( WORLDVIEWPROJ, float4( VSInput.position, 1 ) );
VS_MAIN_END
