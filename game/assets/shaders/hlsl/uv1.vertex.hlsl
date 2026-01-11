#include "ShaderConstants.fxh"

struct VS_Input
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD_0;
};


struct PS_Input
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD_0;
    //float4 color : COLOR;
};


VS_MAIN_BEGIN
    PSInput.uv = VSInput.uv;
    //PSInput.color = VSInput.color;
    PSInput.position = mul( WORLDVIEWPROJ, float4( VSInput.position, 1 ) );
VS_MAIN_END