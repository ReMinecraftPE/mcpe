#include "ShaderConstants.fxh"


struct PS_Input
{
    float4 position : SV_Position;
};

struct PS_Output
{
    float4 color : SV_Target;
};

PS_MAIN_BEGIN
    PSOutput.color = float4(1,1,1,1);
PS_MAIN_END
