
#include "ShaderConstants.fxh"

struct VS_Input {
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD_0;
	float2 uv1 : TEXCOORD_1;
};

struct PS_Input {
	float4 position : SV_Position;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD_0;
	float2 uv1 : TEXCOORD_1;
};

void main(in VS_Input VSInput, out PS_Input PSInput)
{
	PSInput.position = mul(WORLDVIEWPROJ, float4(VSInput.position, 1));

	PSInput.color = VSInput.color;
	PSInput.uv0.x = VSInput.uv0.x*2.0f;
	PSInput.uv0.y = VSInput.uv0.y*10.0f;
	PSInput.uv1 = VSInput.uv1;
}