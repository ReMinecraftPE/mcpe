#include "ShaderConstants.fxh"

struct VS_Input {
	float3 position : POSITION;
	float2 uv0 : TEXCOORD_0;
	float4 color : COLOR;
};

struct PS_Input {
	float4 position : SV_Position;
	float2 uv : TEXCOORD_0;
	float4 color : COLOR;
	float4 worldPosition : TEXCOORD_1;
};

void main(in VS_Input VSInput, out PS_Input PSInput)
{
	float spriteSelector = VSInput.color.x*255.0f;
	PSInput.uv = UV_INFO.xy + (VSInput.uv0 * UV_INFO.zw);
	PSInput.uv.x += spriteSelector * UV_INFO.z;

	float3 position = VSInput.position.xyz;
		
	// subtract the offset then fmod into (0.0f, PARTICLE_BOX)
	position.xyz += POSITION_OFFSET.xyz;
	position.xyz = fmod(position.xyz, PARTICLE_BOX.xxx);
	
	// centre box on origin
	position.xyz -= PARTICLE_BOX.xxx * 0.5f;
	
	// push along view vector so box is positioned more infront of camera
	position.xyz += FORWARD.xyz;
	
	// get world position
	float4 worldPositionBottom = float4(position.xyz, 1.0f);
	float4 worldPositionTop = float4(worldPositionBottom.xyz + (VELOCITY.xyz * SIZE_SCALE.y), 1.0f);

	// get projected positions of top and bottom of particle, and top of particle in previous frame
	float4 bottom = mul(WORLDVIEWPROJ, worldPositionBottom);
	float4 top = mul(WORLDVIEWPROJ, worldPositionTop);
	
	// get 2d vector in screenspace between top and bottom of particle
	float2 dir = (top.xy/top.w) - (bottom.xy/bottom.w);
	
	// get 2d vector perpendicular to velocity
	float2 dirPerp = normalize(float2(-dir.y, dir.x));

	// choose either the top or bottom projected position using uv.y
	PSInput.position = lerp(top, bottom, VSInput.uv0.y);
	 	
	// offset the position of each side of the particle using uv.x 
	PSInput.position.xy += (0.5f - VSInput.uv0.x) * dirPerp * SIZE_SCALE.x;
	
	// get the final colour including the lighting, distance and length scales, and per instance alpha
	PSInput.color = ALPHA;
	
	worldPositionBottom.xz *= 1.0f / 64.0f;	// Scale by 1/TextureDimensions to get values between
	worldPositionBottom.xz += 0.5f;			// Offset so that center of view is in the center of occlusion texture
	worldPositionBottom.y += VIEW_POSITION.y - 0.5f;
	PSInput.worldPosition = worldPositionBottom;
}


