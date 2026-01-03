#include "ShaderConstants.fxh"

struct VS_Input
{
    float3 position : POSITION;
};


struct PS_Input
{
    float4 position : SV_Position;
};


void main( in VS_Input VSInput, out PS_Input PSInput )
{
    PSInput.position = mul( WORLDVIEWPROJ, float4( VSInput.position, 1 ) );
}