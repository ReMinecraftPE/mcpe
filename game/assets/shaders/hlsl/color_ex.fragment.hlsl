#include "ShaderConstants.fxh"


struct PS_Input
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

struct PS_Output
{
    float4 color : SV_Target;
};

PS_MAIN_BEGIN
    PSOutput.color = PSInput.color * CURRENT_COLOR;
PS_MAIN_END
