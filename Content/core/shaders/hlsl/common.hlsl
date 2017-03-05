#ifndef COMMON_HLSL
#define COMMON_HLSL

cbuffer global_PerFrameVariables
{
	float4x4 camera_world; 
	float4x4 camera_view;
	float4x4 camera_view_projection;
	float4x4 camera_projection;
	float4x4 camera_inv_projection;
	float2 camera_near_far;
	float2 back_buffer_size;
};

#endif // COMMON_HLSL
