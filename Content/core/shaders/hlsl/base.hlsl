#ifndef BASE_HLSL
#define BASE_HLSL

#include "common.hlsl"
#include "shader_defines.h"
#include "shadow_mapping.hlsl"

#ifdef DIFFUSE_MAP
Texture2D diffuse_map;
SamplerState diffuse_sampler;
#endif

#ifdef NORMAL_MAP
Texture2D normal_map;
SamplerState normal_sampler;
#endif
 

struct BaseVSInput
{
    float4 position    : POSITION;

#if defined(DIFFUSE_MAP) || defined(NORMAL_MAP)
    float2 tex_coord   : TEXCOORD;
#endif

    float3 normal      : NORMAL;

#if defined(NORMAL_MAP)
    float3 tangent     : TANGENT;
    float3 binormal    : BINORMAL;
#endif

#ifdef INSTANCED
    uint instance_id : SV_InstanceId; 
#endif
};

struct BasePSInput
{
    float4 position : SV_Position;
    float3 position_view : position_view;

#if defined(DIFFUSE_MAP) || defined(NORMAL_MAP)
    float2 tex_coord : tex_coord;
#endif

    float3 normal : normal; // Normal in view space

#if defined(NORMAL_MAP)
    float3 tangent : tangent; // Tangent in view space
    float3 binormal : binormal; // Binormal in view space
#endif
};


#if defined(INSTANCED)
Buffer<float4> instance_data_buffer;
#endif

cbuffer PerObjectVariables
{
    float4x4 world;
};


void BaseVS(BaseVSInput input, out BasePSInput output)
{
    float4x4 instance_world;
    
#if defined(INSTANCED)
    uint data_offset = input.instance_id * 4; // 4 vectors per instance 

    float4 r0 = instance_data_buffer.Load(data_offset);
    float4 r1 = instance_data_buffer.Load(data_offset+1);
    float4 r2 = instance_data_buffer.Load(data_offset+2);
    float4 r3 = instance_data_buffer.Load(data_offset+3);

    instance_world = float4x4(r0, r1, r2, r3);
#else
    instance_world = world;
#endif

    output.position = mul(camera_view_projection, mul(instance_world, input.position));
    output.position_view = mul(camera_view, mul(instance_world, input.position));

#if defined(DIFFUSE_MAP) || defined(NORMAL_MAP)
    output.tex_coord = input.tex_coord;
#endif

    output.normal = mul(camera_view, mul(instance_world, float4(input.normal, 0.0f))).xyz;

#if defined(NORMAL_MAP)
    output.tangent = mul(camera_view, mul(instance_world, float4(input.tangent, 0.0f))).xyz;
    output.binormal = mul(camera_view, mul(instance_world, float4(input.binormal, 0.0f))).xyz;
#endif

}

cbuffer LightParams
{
    float3 sun_light_direction;
    float3 sun_light_color;
};

void BasePS(BasePSInput input, out float4 output : SV_Target0)
{
    float3 normal;
#if defined(NORMAL_MAP)
    float3x3 tangent_frame = float3x3(  normalize(input.tangent),
                                        normalize(input.binormal),
                                        normalize(input.normal)
                                        );
    

    float3 normal_ts = normalize(normal_map.Sample(normal_sampler, input.tex_coord).rgb * 2.0f - 1.0f);
    normal = normalize(mul(transpose(tangent_frame), normal_ts));

#else
    normal = normalize(input.normal);
#endif    
    
    float3 lit_diffuse = float3(0.0f, 0.0f, 0.0f);
    float3 lit_specular = float3(0.0f, 0.0f, 0.0f);

    float3 light_dir = -mul(camera_view, sun_light_direction);
    float3 view_dir = normalize(-input.position_view); 

    float n_dot_l = dot(normal, light_dir);
    if(n_dot_l > 0.0f)
    {
        float3 h = normalize(view_dir + light_dir);
        float cos_th = max(dot(normal, h), 0.0f);
        float cos_ti = max(dot(normal, light_dir), 0.0f);

        float specular = pow(cos_th, 4.0f);

        lit_diffuse += sun_light_color * cos_ti;
        lit_specular += sun_light_color * specular * cos_ti;
    }

#if defined(DIFFUSE_MAP)
    float3 albedo = diffuse_map.Sample(diffuse_sampler, input.tex_coord).rgb;
#else
    float3 albedo = float3(1.0f, 1.0f, 1.0f);
#endif

    uint2 screen_pos = input.position.xy;
    float3 position_ws = mul(camera_world, float4(input.position_view, 1.0f)).xyz;
    float3 shadow_term = ShadowVisibility(position_ws, input.position_view.z, screen_pos);

    output = float4(saturate(shadow_term * albedo * (lit_diffuse + lit_specular * 0.5f)), 1.0f);
}

//--------------------------------------------------------------------------------------
// WriteShadowMap
//--------------------------------------------------------------------------------------

struct DepthVSInput
{
    float4 position : POSITION;
    uint instance_id : SV_InstanceId; 
};

struct DepthGSInput
{
    float4 position_ws  : SV_POSITION;
    uint slice_index    : TEXCOORD1;
};
struct DepthPSInput
{
    float4 position : SV_POSITION;
    uint slice_index : SV_RenderTargetArrayIndex;
};

cbuffer LightInfo
{
    float4x4 light_view_projection[SHADOW_MAPPING_SLICE_COUNT];
};

#ifndef INSTANCED
float slice_index; 
#endif

void DepthVS(DepthVSInput input, out DepthGSInput output)
{   
    float4x4 instance_world;

#ifdef INSTANCED
    uint data_offset = input.instance_id * 5; // 5 vectors per instance 

    float4 r0 = instance_data_buffer.Load(data_offset);
    float4 r1 = instance_data_buffer.Load(data_offset+1);
    float4 r2 = instance_data_buffer.Load(data_offset+2);
    float4 r3 = instance_data_buffer.Load(data_offset+3);

    instance_world = float4x4(r0, r1, r2, r3);
    output.slice_index = instance_data_buffer.Load(data_offset+4).x;
#else
    instance_world = world;
    output.slice_index = slice_index;
#endif

    output.position_ws = mul(light_view_projection[output.slice_index], mul(instance_world, input.position));

}

[maxvertexcount(3)]
void DepthGS(triangle DepthGSInput input[3], inout TriangleStream<DepthPSInput> stream)
{
    DepthPSInput output;
    for(int v = 0; v < 3; ++v)
    {
        output.slice_index = input[v].slice_index;
        output.position = input[v].position_ws;
        stream.Append(output);
    }
}

#endif // BASE_HLSL
