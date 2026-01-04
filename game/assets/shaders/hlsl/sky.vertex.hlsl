#include "ShaderConstants.fxh"

struct VS_Input
{
    float3 position : POSITION;
    float4 color : COLOR;
};


struct PS_Input
{
    float4 position : SV_Position;
    float4 color : COLOR;
};


void main( in VS_Input VSInput, out PS_Input PSInput )
{
    PSInput.position = mul( WORLDVIEWPROJ, float4( VSInput.position, 1 ) );

    PSInput.color = lerp( CURRENT_COLOR, FOG_COLOR, VSInput.color.r );
}