#include "ShaderConstants.fxh"

struct VS_Input
{
    float3 position : POSITION;
    float2 uv : TEXCOORD_0;
};


struct PS_Input
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD_0;
};


VS_MAIN_BEGIN
    PSInput.uv = VSInput.uv;
    PSInput.position = mul( WORLDVIEWPROJ, float4( VSInput.position, 1 ) );
VS_MAIN_END