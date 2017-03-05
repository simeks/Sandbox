#ifndef LINE_DRAWER_HLSL
#define LINE_DRAWER_HLSL

#include "common.hlsl"

struct VSInput
{
	float3 position : POSITION;
	uint4 color		: COLOR;
};

struct VSOutput 
{
	float4 position_cs : SV_Position;
	float4 color		: COLOR;
};


VSOutput VSMain( in VSInput input )
{
	VSOutput output;
	float4 position = float4(input.position, 1.0f);
	position.xy /= back_buffer_size.xy;
	position.xy = position.xy * 2 - 1;
	position.y *= -1;
	
	output.position_cs = position;
	output.color = input.color/255.0f;
	return output;
}
float4 PSMain( in VSOutput input ) : SV_Target0
{
	return input.color;
}



#endif // LINE_DRAWER_HLSL
