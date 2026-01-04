#include "ShaderConstants.fxh"


float4 glintBlend(float4 dest, float4 source) {
	return float4(source.rgb * source.rgb, 0.0) + dest;
}

struct PS_Input
{
    float4 position : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD_0;
#ifdef GLINT
	float2 layer1UV : UV_1;
	float2 layer2UV : UV_2;
#endif
};

struct PS_Output
{
    float4 color : SV_Target;
};

void main( in PS_Input PSInput, out PS_Output PSOutput )
{
    float4 diffuse = TEXTURE_0.Sample( TextureSampler0, PSInput.uv );

#ifdef ALPHA_TEST
    if( diffuse.a < 0.5 )
    {
        discard;
    }
#endif

#ifdef GLINT
	float4 layer1 = TEXTURE_1.Sample(TextureSampler1, frac(PSInput.layer1UV)).rgbr * CHANGE_COLOR;
	float4 layer2 = TEXTURE_1.Sample(TextureSampler1, frac(PSInput.layer2UV)).rgbr * CHANGE_COLOR;
	float4 glint = (layer1 + layer2);

	#ifdef INVENTORY
		diffuse = glint;
	#else
		diffuse = glintBlend(diffuse, glint);
	#endif
#endif

#ifdef USE_OVERLAY
        //use either the diffuse or the OVERLAY_COLOR
    diffuse.rgb = lerp( diffuse, OVERLAY_COLOR, OVERLAY_COLOR.a ).rgb;
#endif

    PSOutput.color = diffuse * PSInput.color;
}