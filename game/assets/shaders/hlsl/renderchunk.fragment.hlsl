#include "ShaderConstants.fxh"
#include "renderchunk.fxh"

struct PS_Input
{
    float4 position : SV_Position;
    lpfloat4 color : COLOR;
    snorm float2 uv0 : TEXCOORD_0;
    //snorm float2 uv1 : TEXCOORD_1;
#ifdef FOG
    float4 fogColor : PS_FOG_COLOR;
#endif
};

struct PS_Output
{
    float4 color : SV_Target;
};

PS_MAIN_BEGIN
	//float4 diffuse = TEXTURE_0.Sample( TextureSampler0, PSInput.uv0 ) * TEXTURE_1.Sample( TextureSampler1, PSInput.uv1 );
	float4 diffuse = TEXTURE_0.Sample( TextureSampler0, PSInput.uv0 );

#ifdef SEASONS_FAR
	diffuse.a = 1.0f;
	PSInput.color.b = 1.0f;
#endif

#ifdef ALPHA_TEST
    if( diffuse.a < 0.5 )
    {
        discard;
    }
#endif

#ifndef SEASONS

#if !defined(ALPHA_TEST) && !defined(BLEND)
	PSInput.color.rgb = lerp(PSInput.color.aaa, PSInput.color.rgb, diffuse.a);
#elif defined(BLEND)
	diffuse.a *= PSInput.color.a;
#endif	
	diffuse.rgb *= PSInput.color.rgb;
#else
	float2 uv = PSInput.color.xy;
	uv.y += 1.0f / 512.0f;
	diffuse.rgb *= lerp(1.0f, TEXTURE_2.Sample(TextureSampler2, uv).rgb*2.0f, PSInput.color.b);
	diffuse.rgb *= PSInput.color.aaa;
	diffuse.a = 1.0f;
#endif

#ifdef FOG
    diffuse.rgb = lerp( diffuse.rgb, PSInput.fogColor.rgb, PSInput.fogColor.a );
#endif

    PSOutput.color = diffuse;
PS_MAIN_END
