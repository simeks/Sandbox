#include "common.hlsl"

struct VSInput
{
	float3 position : POSITION;
	float2 uv		: TEXCOORD;
	uint4 color		: COLOR;
};


struct VSOutput 
{
	float4 position_cs	: SV_Position;
	float2 uv			: TEXCOORD;
	float4 color		: COLOR;
};
struct PSOutput
{
	float4 color : SV_Target0;
};


#if TEXTURED || TEXT
Texture2D diffuse_map;

#elif TEXTURE_ARRAY

Texture2DArray diffuse_map;

float tex_array_index;

#endif

SamplerState diffuse_sampler;


VSOutput VSMain( in VSInput input )
{
	VSOutput output;
	float4 position = float4(input.position, 1.0f);
	position.xy /= back_buffer_size.xy;
	position.xy = position.xy * 2 - 1;
	position.y *= -1;
	
	output.position_cs = position;
	output.uv = input.uv;
	output.color = input.color/255.0f;
	return output;
}

float4 PSMain( in VSOutput input ) : SV_Target0
{
#ifdef TEXTURED
	return float4(diffuse_map.Sample(diffuse_sampler, input.uv).rgb * input.color.rgb, 1.0f);
#elif TEXT
	float a = diffuse_map.Sample(diffuse_sampler, input.uv).r;
	return float4(1.0f, 1.0f, 1.0f, a) * input.color;
#elif TEXTURE_ARRAY
	return float4(diffuse_map.Sample(diffuse_sampler, float3(input.uv, tex_array_index)).x * input.color.rgb, 1.0f);
#else
	return input.color;
#endif
}

