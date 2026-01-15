#include "ShaderConstants.fxh"
#include "entity.fxh"

struct PS_Input
{
    float4 position : SV_Position;

    float4 light : LIGHT;
    float4 fogColor : PS_FOG_COLOR;

    float2 uv : TEXCOORD_0;

#ifdef USE_OVERLAY
    float4 overlayColor : PS_OVERLAY_COLOR;
#endif

#ifdef GLINT
	float2 layer1UV : UV_1;
	float2 layer2UV : UV_2;
#endif
};

struct PS_Output
{
    float4 color : SV_Target;
};

#ifdef USE_EMISSIVE
#define NEEDS_DISCARD(C)	(C.a + C.r + C.g + C.b == 0.0)
#else
#define NEEDS_DISCARD(C)	(C.a < 0.5)
#endif

float4 glintBlend(float4 dest, float4 source) {
	// glBlendFuncSeparate(GL_SRC_COLOR, GL_ONE, GL_ONE, GL_ZERO)
	return float4(source.rgb * source.rgb, source.a) + float4(dest.rgb, 0.0);
}

PS_MAIN_BEGIN
    float4 color = float4( 1.0f, 1.0f, 1.0f, 1.0f );

#ifndef COLOR_BASED
        color = TEXTURE_0.Sample( TextureSampler0, PSInput.uv );

#ifdef ALPHA_TEST
        if( NEEDS_DISCARD( color ) )
        {
            discard;
        }
#endif
#endif

#ifdef USE_COLOR_MASK
	#ifdef GLINT
		// Applies color mask to glint texture instead and blends with original color
		float4 layer1 = TEXTURE_1.Sample(TextureSampler1, frac(PSInput.layer1UV)).rgbr * CHANGE_COLOR;
		float4 layer2 = TEXTURE_1.Sample(TextureSampler1, frac(PSInput.layer2UV)).rgbr * CHANGE_COLOR;
		float4 glint = (layer1 + layer2) * TILE_LIGHT_COLOR;
	#else
		color.rgb = lerp( color, color * CHANGE_COLOR, color.a ).rgb;
		color.a = 1.0;
	#endif
#endif

#ifdef USE_OVERLAY
        //use either the diffuse or the OVERLAY_COLOR
        color.rgb = lerp( color, PSInput.overlayColor, PSInput.overlayColor.a ).rgb;
#endif

#ifdef USE_EMISSIVE
        //make glowy stuff
        color *= lerp( float( 1.0 ).xxxx, PSInput.light, color.a );
#else
        color *= PSInput.light;
#endif

    //apply fog
    color.rgb = lerp( color.rgb, PSInput.fogColor.rgb, PSInput.fogColor.a );

#ifdef USE_COLOR_MASK
	#ifdef GLINT
		color = glintBlend(color, glint);
	#endif
#endif

    //WARNING do not refactor this 
    PSOutput.color = color;
PS_MAIN_END
