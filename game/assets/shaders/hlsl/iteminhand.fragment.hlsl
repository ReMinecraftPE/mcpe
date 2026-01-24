#include "ShaderConstants.fxh"


struct PS_Input
{
    float4 position : SV_Position;
    float4 light : LIGHT;
};

struct PS_Output
{
    float4 color : SV_Target;
};

PS_MAIN_BEGIN
    float4 color = lerp( float4( 1, 1, 1, 1 ), PSInput.light, PSInput.light.a );

    color.rgb = lerp( color, OVERLAY_COLOR, OVERLAY_COLOR.a ).rgb;

    PSOutput.color = color;
PS_MAIN_END
