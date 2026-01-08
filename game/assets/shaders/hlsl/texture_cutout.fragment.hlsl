#include "ShaderConstants.fxh"


struct PS_Input
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD_0;
};

struct PS_Output
{
    float4 color : SV_Target;
};

void main( in PS_Input PSInput, out PS_Output PSOutput )
{
    float4 diffuse = TEXTURE_0.Sample( TextureSampler0, PSInput.uv );


    if( diffuse.a < 0.5 )
    {
        discard;
    }

    PSOutput.color = CURRENT_COLOR;
}