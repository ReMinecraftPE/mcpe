#include "ShaderConstants.fxh"

struct VS_Input
{
	float3 position : POSITION;
};


struct PS_Input
{
	float4 position : SV_Position;
};

VS_MAIN_BEGIN
	PSInput.position = mul(WORLDVIEWPROJ, float4(VSInput.position, 1));

    //this is to make it come closer to the camera and avoid z-fighting
    PSInput.position.z -= 0.0001;
VS_MAIN_END
