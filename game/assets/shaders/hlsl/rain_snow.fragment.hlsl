#include "ShaderConstants.fxh"

struct PS_Input {
	float4 position : SV_Position;
	float2 uv : TEXCOORD_0;
	float4 color : COLOR;
	float4 worldPosition : TEXCOORD_1;
};

struct PS_Output {
	float4 color : SV_Target;
};

void main( in PS_Input PSInput, out PS_Output PSOutput ) {
	
	PSOutput.color = TEXTURE_0.Sample(TextureSampler0, PSInput.uv);

#ifdef ALPHA_TEST
	if (PSOutput.color.a < 0.5)
		discard;
#endif

	PSOutput.color.a *= PSInput.color.a;

	float2 uv = PSInput.worldPosition.xz;
	float4 occlusionTexture = TEXTURE_1.Sample(TextureSampler1, uv);

	occlusionTexture.a *= 255.0f;	// Alpha stores height of world

	// clamp the uvs
	float2 uvclamp = saturate(uv);
	if (uvclamp.x == uv.x && uvclamp.y == uv.y && PSInput.worldPosition.y < occlusionTexture.a) {
		discard;
	}

	float mixAmount = saturate((PSInput.worldPosition.y - occlusionTexture.a)*0.1f);
	float3 lighting = lerp(occlusionTexture.rgb, PSInput.color.rgb, mixAmount);
	PSOutput.color.rgb *= lighting.rgb;
}


