#include "ShaderConstants.fxh"

struct VS_Input
{
    float4 position : POSITION;
    float4 color : COLOR;
};


struct PS_Input
{
    float4 position : SV_Position;
    float4 color : COLOR;
};


VS_MAIN_BEGIN
    PSInput.color = VSInput.color;
    PSInput.position = mul( WORLDVIEWPROJ, VSInput.position );
VS_MAIN_END
