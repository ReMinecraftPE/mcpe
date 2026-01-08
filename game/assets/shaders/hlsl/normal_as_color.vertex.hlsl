#include "ShaderConstants.fxh"

struct VS_Input
{
    float3 position : POSITION;
    float4 normal : NORMAL;
};


struct PS_Input
{
    float4 position : SV_Position;
    float4 color : COLOR;
};


void main( in VS_Input VSInput, out PS_Input PSInput )
{
    PSInput.position = mul( WORLDVIEWPROJ, float4( VSInput.position, 1 ) );

    PSInput.color.rgb = ( VSInput.normal.xyz / 2 ) + 0.5;
    PSInput.color.a = 1.0;
}