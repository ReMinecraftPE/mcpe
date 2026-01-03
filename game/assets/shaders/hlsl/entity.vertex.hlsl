#include "ShaderConstants.fxh"


struct VS_Input
{
    float3 position : POSITION;
    float4 normal : NORMAL;
    float2 texCoords : TEXCOORD_0;
#ifdef COLOR_BASED
    float4 color : COLOR;
#endif
};


struct PS_Input
{
    float4 position : SV_Position;

    float4 light : LIGHT;
    float4 fogColor : FOG_COLOR;

    float2 uv : TEXCOORD_0;

#ifdef USE_OVERLAY
    float4 overlayColor : OVERLAY_COLOR;
#endif
#ifdef GLINT
	float2 layer1UV : UV_1;
	float2 layer2UV : UV_2;
#endif
};

static const float AMBIENT = 0.45;

static const float XFAC = -0.1;
static const float ZFAC = 0.1;


float4 TransformRGBA8_SNORM( const float4 RGBA8_SNORM )
{
#ifdef R8G8B8A8_SNORM_UNSUPPORTED
    return RGBA8_SNORM * float( 2.0 ).xxxx - float( 1.0 ).xxxx;
#else
    return RGBA8_SNORM;
#endif
}


float lightIntensity( const float4 position, const float4 normal )
{
#ifdef FANCY
    float3 N = normalize( mul( WORLD, normal ) ).xyz;

        //take care of double sided polygons on materials without culling
#ifdef FLIP_BACKFACES
        float3 viewDir = normalize( ( mul( WORLD, position ) ).xyz );
        if( dot( N, viewDir ) > 0.0 )
        {
            N *= -1.0;
        }
#endif

        float yLight = ( 1.0 + N.y ) * 0.5;
        return yLight * ( 1.0 - AMBIENT ) + N.x*N.x * XFAC + N.z*N.z * ZFAC + AMBIENT;
#else
    return 1.0;
#endif
}

#ifdef GLINT
float2 calculateLayerUV(float2 origUV, float offset, float rotation) {
	float2 uv = origUV;
	uv -= 0.5;
	float rsin = sin(rotation);
	float rcos = cos(rotation);
	uv = mul(uv, float2x2(rcos, -rsin, rsin, rcos));
	uv.x += offset;
	uv += 0.5;

	return uv * GLINT_UV_SCALE;
}
#endif

void main( in VS_Input VSInput, out PS_Input PSInput )
{
    PSInput.position = mul( WORLDVIEWPROJ, float4( VSInput.position, 1 ) );
    float4 normal = TransformRGBA8_SNORM( VSInput.normal );

    float L = lightIntensity( float4( VSInput.position, 1 ), normal );

#ifdef USE_OVERLAY
    L += OVERLAY_COLOR.a * 0.35;
    PSInput.overlayColor = OVERLAY_COLOR;
#endif

    PSInput.light = float4( L.xxx * TILE_LIGHT_COLOR.rgb, 1.0 );

#ifdef COLOR_BASED
    PSInput.light *= VSInput.color;
#else
    PSInput.uv = VSInput.texCoords;
#endif

#ifdef USE_UV_ANIM
	PSInput.uv.xy += UV_ANIM.xy;
#endif

#ifdef GLINT
	PSInput.layer1UV = calculateLayerUV(VSInput.texCoords, UV_OFFSET.x, UV_ROTATION.x);
	PSInput.layer2UV = calculateLayerUV(VSInput.texCoords, UV_OFFSET.y, UV_ROTATION.y);
#endif

    //fog
    PSInput.fogColor.rgb = FOG_COLOR.rgb;
    PSInput.fogColor.a = clamp( ( ( PSInput.position.z / RENDER_DISTANCE ) - FOG_CONTROL.x ) / ( FOG_CONTROL.y - FOG_CONTROL.x ), 0.0, 1.0 );
}

