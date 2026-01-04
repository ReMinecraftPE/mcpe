#include "ShaderConstants.fxh"

struct PS_Input {
	float4 position : SV_Position;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD_0;
	float2 uv1 : TEXCOORD_1;
};

struct PS_Output {
	float4 color : SV_Target;
};

void main(in PS_Input PSInput, out PS_Output PSOutput) 
{
	float4 diffuse = TEXTURE_0.Sample(TextureSampler0, PSInput.uv0) * TEXTURE_1.Sample(TextureSampler1, PSInput.uv1);

#ifdef ALPHA_TEST
	if(diffuse.a < 0.5)
	 	discard;
#endif
	diffuse *= PSInput.color;

	PSOutput.color = diffuse;
}
