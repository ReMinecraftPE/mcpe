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

PS_MAIN_BEGIN
	float4 diffuse = sampleTex0(TextureSampler0, PSInput.uv0) * sampleTex1(TextureSampler1, PSInput.uv1);

#ifdef ALPHA_TEST
	if(diffuse.a < 0.5)
	 	discard;
#endif
	diffuse *= PSInput.color;

	PSOutput.color = diffuse;
PS_MAIN_END
