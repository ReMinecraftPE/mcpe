#include "ShaderConstants.fxh"


struct PS_Input
{
    float4 position : SV_Position;
};

struct PS_Output
{
    float4 color : SV_Target;
};

void main( in PS_Input PSInput, out PS_Output PSOutput )
{
    PSOutput.color = float4(1,1,1,1);
}