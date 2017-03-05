#ifndef SHADOW_MAPPING_HLSL
#define SHADOW_MAPPING_HLSL


//--------------------------------------------------------------------------------------
// ReadShadowMap
//--------------------------------------------------------------------------------------

Texture2DArray cascaded_shadow_map;
SamplerComparisonState shadow_sampler_pcf;

cbuffer ShadowConstants
{
	float4x4 shadow_matrix;
	float3 cascade_offsets[SHADOW_MAPPING_SLICE_COUNT];
	float3 cascade_scales[SHADOW_MAPPING_SLICE_COUNT];
	float2 min_max_border_padding;
};


float2 ComputeReceiverPlaneDepthBias(float3 tex_coord_dx, float3 tex_coord_dy)
{
    float2 bias_uv;
    bias_uv.x = tex_coord_dy.y * tex_coord_dx.z - tex_coord_dx.y * tex_coord_dy.z;
    bias_uv.y = tex_coord_dx.x * tex_coord_dy.z - tex_coord_dy.x * tex_coord_dx.z;
    bias_uv *= 1.0f / ((tex_coord_dx.x * tex_coord_dy.y) - (tex_coord_dx.y * tex_coord_dy.x));
    return bias_uv;
}

float SampleShadowMap(float2 base_uv, float u, float v, float2 shadow_map_size_inv, uint cascade_idx, float depth, float2 receiver_plane_depth_bias) {

    float2 uv = base_uv + float2(u, v) * shadow_map_size_inv;

    float z = depth + dot(float2(u, v) * shadow_map_size_inv, receiver_plane_depth_bias);

    return cascaded_shadow_map.SampleCmpLevelZero(shadow_sampler_pcf, float3(uv, cascade_idx), z);
}

//-------------------------------------------------------------------------------------------------
// The method used in The Witness
//-------------------------------------------------------------------------------------------------
float SampleShadowMapOptimizedPCF(in float3 shadow_pos, in float3 shadow_pos_dx,
						 in float3 shadow_pos_dy, in uint cascade_idx) {
	float2 shadow_map_size;
	float num_slices;
	cascaded_shadow_map.GetDimensions(shadow_map_size.x, shadow_map_size.y, num_slices);

	float light_depth = shadow_pos.z;

	float2 texel_size = 1.0f / shadow_map_size;

	float2 receiver_plane_depth_bias = ComputeReceiverPlaneDepthBias(shadow_pos_dx, shadow_pos_dy);

	// Static depth biasing to make up for incorrect fractional sampling on the shadow map grid
	float fractional_sampling_error = 2 * dot(float2(1.0f, 1.0f) * texel_size, abs(receiver_plane_depth_bias));
	light_depth -= min(fractional_sampling_error, 0.01f);


	float2 uv = shadow_pos.xy * shadow_map_size; // 1 unit - 1 texel

	float2 shadow_map_size_inv = 1.0 / shadow_map_size;

	float2 base_uv;
	base_uv.x = floor(uv.x + 0.5);
	base_uv.y = floor(uv.y + 0.5);

	float s = (uv.x + 0.5 - base_uv.x);
	float t = (uv.y + 0.5 - base_uv.y);

	base_uv -= float2(0.5, 0.5);
	base_uv *= shadow_map_size_inv;

	float sum = 0;

#if PCF_FILTER_SIZE == 2
	return cascaded_shadow_map.SampleCmpLevelZero(shadow_sampler_pcf, float3(shadow_pos.xy, cascade_idx), light_depth);
#elif PCF_FILTER_SIZE == 3

	float uw0 = (3 - 2 * s);
	float uw1 = (1 + 2 * s);

	float u0 = (2 - s) / uw0 - 1;
	float u1 = s / uw1 + 1;

	float vw0 = (3 - 2 * t);
	float vw1 = (1 + 2 * t);

	float v0 = (2 - t) / vw0 - 1;
	float v1 = t / vw1 + 1;

	sum += uw0 * vw0 * SampleShadowMap(base_uv, u0, v0, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);
	sum += uw1 * vw0 * SampleShadowMap(base_uv, u1, v0, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);
	sum += uw0 * vw1 * SampleShadowMap(base_uv, u0, v1, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);
	sum += uw1 * vw1 * SampleShadowMap(base_uv, u1, v1, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);

	return sum * 1.0f / 16;

#elif PCF_FILTER_SIZE == 5

	float uw0 = (4 - 3 * s);
	float uw1 = 7;
	float uw2 = (1 + 3 * s);

	float u0 = (3 - 2 * s) / uw0 - 2;
	float u1 = (3 + s) / uw1;
	float u2 = s / uw2 + 2;

	float vw0 = (4 - 3 * t);
	float vw1 = 7;
	float vw2 = (1 + 3 * t);

	float v0 = (3 - 2 * t) / vw0 - 2;
	float v1 = (3 + t) / vw1;
	float v2 = t / vw2 + 2;

	sum += uw0 * vw0 * SampleShadowMap(base_uv, u0, v0, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);
	sum += uw1 * vw0 * SampleShadowMap(base_uv, u1, v0, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);
	sum += uw2 * vw0 * SampleShadowMap(base_uv, u2, v0, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);

	sum += uw0 * vw1 * SampleShadowMap(base_uv, u0, v1, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);
	sum += uw1 * vw1 * SampleShadowMap(base_uv, u1, v1, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);
	sum += uw2 * vw1 * SampleShadowMap(base_uv, u2, v1, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);

	sum += uw0 * vw2 * SampleShadowMap(base_uv, u0, v2, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);
	sum += uw1 * vw2 * SampleShadowMap(base_uv, u1, v2, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);
	sum += uw2 * vw2 * SampleShadowMap(base_uv, u2, v2, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);

	return sum * 1.0f / 144;

#else // PCF_FILTER_SIZE == 7

	float uw0 = (5 * s - 6);
	float uw1 = (11 * s - 28);
	float uw2 = -(11 * s + 17);
	float uw3 = -(5 * s + 1);

	float u0 = (4 * s - 5) / uw0 - 3;
	float u1 = (4 * s - 16) / uw1 - 1;
	float u2 = -(7 * s + 5) / uw2 + 1;
	float u3 = -s / uw3 + 3;

	float vw0 = (5 * t - 6);
	float vw1 = (11 * t - 28);
	float vw2 = -(11 * t + 17);
	float vw3 = -(5 * t + 1);

	float v0 = (4 * t - 5) / vw0 - 3;
	float v1 = (4 * t - 16) / vw1 - 1;
	float v2 = -(7 * t + 5) / vw2 + 1;
	float v3 = -t / vw3 + 3;

	sum += uw0 * vw0 * SampleShadowMap(base_uv, u0, v0, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);
	sum += uw1 * vw0 * SampleShadowMap(base_uv, u1, v0, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);
	sum += uw2 * vw0 * SampleShadowMap(base_uv, u2, v0, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);
	sum += uw3 * vw0 * SampleShadowMap(base_uv, u3, v0, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);

	sum += uw0 * vw1 * SampleShadowMap(base_uv, u0, v1, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);
	sum += uw1 * vw1 * SampleShadowMap(base_uv, u1, v1, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);
	sum += uw2 * vw1 * SampleShadowMap(base_uv, u2, v1, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);
	sum += uw3 * vw1 * SampleShadowMap(base_uv, u3, v1, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);

	sum += uw0 * vw2 * SampleShadowMap(base_uv, u0, v2, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);
	sum += uw1 * vw2 * SampleShadowMap(base_uv, u1, v2, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);
	sum += uw2 * vw2 * SampleShadowMap(base_uv, u2, v2, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);
	sum += uw3 * vw2 * SampleShadowMap(base_uv, u3, v2, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);

	sum += uw0 * vw3 * SampleShadowMap(base_uv, u0, v3, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);
	sum += uw1 * vw3 * SampleShadowMap(base_uv, u1, v3, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);
	sum += uw2 * vw3 * SampleShadowMap(base_uv, u2, v3, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);
	sum += uw3 * vw3 * SampleShadowMap(base_uv, u3, v3, shadow_map_size_inv, cascade_idx, light_depth, receiver_plane_depth_bias);

	return sum * 1.0f / 2704;

#endif
}


//-------------------------------------------------------------------------------------------------
// Samples the appropriate shadow map cascade
//-------------------------------------------------------------------------------------------------
float3 SampleShadowCascade(in float3 shadow_position, in float3 shadow_pos_dx,
						   in float3 shadow_pos_dy, in uint cascade_idx,
						   in uint2 screen_pos)
{

	float shadow = SampleShadowMapOptimizedPCF(shadow_position, shadow_pos_dx, shadow_pos_dy, cascade_idx);
	
	return shadow;
}

//-------------------------------------------------------------------------------------------------
// Computes the visibility term by performing the shadow test
//-------------------------------------------------------------------------------------------------
float3 ShadowVisibility(in float3 position_ws, in float depth_vs,
						in uint2 screen_pos)
{
	float3 shadow_visibility = 1.0f;
	uint cascade_idx = 0;
	

	// Project into shadow space
	float3 sample_pos = position_ws;
	float3 shadow_position = mul(shadow_matrix, float4(sample_pos, 1.0f)).xyz;
	float3 shadow_pos_dx = ddx_fine(shadow_position);
	float3 shadow_pos_dy = ddy_fine(shadow_position);

	float3 shadow_map_texture_coord = 0;
	if(SHADOW_MAPPING_SLICE_COUNT == 1)
	{
		shadow_map_texture_coord = shadow_position + cascade_offsets[0].xyz;
		shadow_map_texture_coord *= cascade_scales[0].xyz;
	}
	else if(SHADOW_MAPPING_SLICE_COUNT > 1)
	{
		int cascade_found = 0;
		for(uint i = 0; i < SHADOW_MAPPING_SLICE_COUNT && cascade_found == 0; ++i)
		{
			shadow_map_texture_coord = shadow_position + cascade_offsets[i].xyz;
			shadow_map_texture_coord *= cascade_scales[i].xyz;

			if ( min( shadow_map_texture_coord.x, shadow_map_texture_coord.y ) > min_max_border_padding.x
				&& max( shadow_map_texture_coord.x, shadow_map_texture_coord.y ) < min_max_border_padding.y )
			{ 
				cascade_idx = i;   
				cascade_found = 1; 
			}
		}
	}
	
	shadow_pos_dx *= cascade_scales[cascade_idx].xyz;
	shadow_pos_dy *= cascade_scales[cascade_idx].xyz;

#ifdef VISUALIZE_CASCADES
	float3 cascade_colors[] = { {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};

	shadow_visibility = cascade_colors[cascade_idx]; 
#else
	shadow_visibility = SampleShadowCascade(shadow_map_texture_coord, shadow_pos_dx, shadow_pos_dy,
										   cascade_idx, screen_pos);
#endif


	return shadow_visibility;
}

#endif // SHADOW_MAPPING_HLSL
