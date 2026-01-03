#include "ShaderConstants.fxh"

struct VS_Input
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 texCoords : TEXCOORD_0;
};


struct PS_Input
{
    float4 position : SV_Position;
    float4 color : COLOR;
    float2 texCoords : TEXCOORD_0;
#ifdef GLINT
	float2 layer1UV : UV_1;
	float2 layer2UV : UV_2;
#endif
};

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
    PSInput.color = VSInput.color;
    PSInput.position = mul( WORLDVIEWPROJ, float4( VSInput.position, 1 ) );
    PSInput.texCoords = VSInput.texCoords;
#ifdef GLINT
	PSInput.layer1UV = calculateLayerUV(VSInput.texCoords, UV_OFFSET.x, UV_ROTATION.x);
	PSInput.layer2UV = calculateLayerUV(VSInput.texCoords, UV_OFFSET.y, UV_ROTATION.y);
#endif
}