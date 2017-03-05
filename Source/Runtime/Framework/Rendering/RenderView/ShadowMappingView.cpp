// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "ShadowMappingView.h"
#include "World/RenderWorld.h"
#include "World/RenderComponent.h"
#include "World/MeshComponent.h"
#include "World/Camera.h"
#include "Rendering/Layer.h"
#include "Rendering/RenderResourceSet.h"

#include <Foundation/Thread/TaskScheduler.h>
#include <Foundation/Math/MatrixUtil.h>
#include <Foundation/Container/ConfigValue.h>
#include <Foundation/Profiler/Profiler.h>

#include <Engine/Rendering/RRenderTarget.h>
#include <Engine/Rendering/ShaderParameters.h>
#include <Engine/Rendering/RenderContext.h>

#include <Content/core/shaders/hlsl/shader_defines.h>

namespace sb
{

	namespace shadow_mapping
	{
		struct UpdateSlicesJobData
		{
			ShadowSlice* slices;
			float shadow_map_size;

			Vec3f* frustum_corners;
			Mat4x4f camera_world_matrix;

			Vec3f light_direction;
		};

		void UpdateSlicesKernel(void* data, const Range& range);
	}

	void shadow_mapping::UpdateSlicesKernel(void* data, const Range& range)
	{
		MICROPROFILE_SCOPEI("Main", "UpdateSlicesKernel", 0xFF3322FF);

		UpdateSlicesJobData* job_data = (UpdateSlicesJobData*)data;

		float frustum_interval_begin, frustum_interval_end;
		for (int i = range.begin; i < range.end; ++i)
		{
			ShadowSlice& slice = job_data->slices[i];

			frustum_interval_begin = slice.depth_begin;
			frustum_interval_end = slice.depth_end;

			Vec3f frustum_corners_ws[8];
			Vec3f frustum_center_ws = Vec3f(0.0f, 0.0f, 0.0f);

			// Modify the near and far values of the original frustum to the cascade intervals.
			//	4 first corners are the near plane, 4 last are the far plane.
			for (uint32_t c = 0; c < 4; ++c)
			{
				// Transform the frustum from camera view space to world space
				frustum_corners_ws[c] = job_data->camera_world_matrix * job_data->frustum_corners[c];
				frustum_corners_ws[c + 4] = job_data->camera_world_matrix * job_data->frustum_corners[c + 4];

				Vec3f corner_ray = frustum_corners_ws[c + 4] - frustum_corners_ws[c];
				corner_ray.Normalize();

				frustum_corners_ws[c] = frustum_corners_ws[c] + (corner_ray * frustum_interval_begin);
				frustum_corners_ws[c + 4] = frustum_corners_ws[c] + (corner_ray * frustum_interval_end);

				frustum_center_ws += frustum_corners_ws[c] + frustum_corners_ws[c + 4];
			}

			frustum_center_ws /= 8.0f;

			Vec3f up_dir = Vec3f(0.0f, -1.0f, 0.0f);

			float sphere_radius = 0.0f;
			for (uint32_t c = 0; c < 8; ++c)
			{
				float dist = (frustum_corners_ws[c] - frustum_center_ws).Length();
				sphere_radius = Max(sphere_radius, dist);
			}
			sphere_radius = math::Ceil(sphere_radius * 16.0f) / 16.0f;

			Vec3f shadow_camera_position = frustum_center_ws - job_data->light_direction * sphere_radius * 1.75f; // TODO: Find correct far plane, best would be to use scene bounds.

			Mat4x4f light_proj = matrix_util::CreateOrthoOffCenter(-sphere_radius, sphere_radius,
																   -sphere_radius, sphere_radius,
																   0.0f, 3.0f*sphere_radius); // TODO: Find correct far plane, best would be to use scene bounds.


			Mat4x4f light_view = matrix_util::CreateLookAt(shadow_camera_position, frustum_center_ws, up_dir);

			Mat4x4f shadow_matrix = light_proj * light_view;
			Vec3f shadow_origin = Vec3f(0.0f, 0.0f, 0.0f);
			shadow_origin = shadow_matrix * shadow_origin;
			shadow_origin = shadow_origin * (job_data->shadow_map_size * 0.5f);

			Vec3f rounded_origin = Vec3f(math::Round(shadow_origin.x), math::Round(shadow_origin.y), math::Round(shadow_origin.z));
			Vec3f round_offset = rounded_origin - shadow_origin;
			round_offset = round_offset * (2.0f / job_data->shadow_map_size);

			light_proj.m03 += round_offset.x;
			light_proj.m13 += round_offset.y;
			light_proj.m23 += round_offset.z;

			slice.shadow_projection = light_proj;
			slice.shadow_view = light_view;

		}
	}


	Mat4x4f shadow_mapping::CreateGlobalShadowMatrix(const Camera* camera, const Vec3f& light_direction)
	{
		// Pick the up vector to use for the light camera
		Vec3f up_dir = Vec3f(0.0f, -1.0f, 0.0f);

		Vec3f frustum_corners[8];
		camera->CalculateFrustumCorners(frustum_corners);

		Vec3f frustum_center(0, 0, 0);

		// Transform the frustum from camera view space to world space
		Mat4x4f camera_world_matrix = camera->GetWorldMatrix();
		for (uint32_t i = 0; i < 8; ++i)
		{
			frustum_corners[i] = camera_world_matrix * frustum_corners[i];
			frustum_center += frustum_corners[i];
		}
		frustum_center /= 8;

		Vec3f shadow_cam_pos = frustum_center - light_direction;

		Mat4x4f light_view = matrix_util::CreateLookAt(shadow_cam_pos, frustum_center, up_dir);

		return light_view;
	}

	void shadow_mapping::CalculateCascadeOffsetAndScale(const Mat4x4f& shadow_view_matrix, 
														const Mat4x4f& shadow_proj_matrix,
														const Mat4x4f& global_view_matrix, 
														Vec3f& out_offset, 
														Vec3f& out_scale)
	{
		Mat4x4f tex_scale_bias;
		tex_scale_bias.SetScale(Vec3f(0.5f, -0.5f, 1.0f));
		tex_scale_bias.SetTranslation(Vec3f(0.5f, 0.5f, 0.0f));

		Mat4x4f inv_shadow_view_proj = tex_scale_bias * shadow_proj_matrix * shadow_view_matrix;
		inv_shadow_view_proj.Invert();

		Vec3f lower_corner = inv_shadow_view_proj * Vec3f(0.0f, 0.0f, 0.0f);
		lower_corner = global_view_matrix * lower_corner;

		Vec3f upper_corner = inv_shadow_view_proj * Vec3f(1.0f, 1.0f, 1.0f);
		upper_corner = global_view_matrix * upper_corner;

		Vec3f extent = upper_corner - lower_corner;
		out_scale.x = 1.0f / extent.x;
		out_scale.y = 1.0f / extent.y;
		out_scale.z = 1.0f / extent.z;

		out_offset = -lower_corner;
	}

	ShadowMappingView::ShadowMappingView()
		: _shadow_map_size(0),
		  _pcf_filter_size(0)
	{

	}
	ShadowMappingView::~ShadowMappingView()
	{

	}
	void ShadowMappingView::Load(const ConfigValue& config, RenderResourceAllocator*, RenderResourceSet*)
	{
		Assert(config.IsObject());
		_slices.resize(SHADOW_MAPPING_SLICE_COUNT);

		Assert(config["light_direction_var"].IsString());
		_light_direction_variable = config["light_direction_var"].AsString();
	}
	void ShadowMappingView::Unload(RenderResourceAllocator*)
	{

	}
	void ShadowMappingView::Render(uint64_t sort_key, const Params& params, RenderContext* render_context)
	{
		RRenderTarget* depth_target = (RRenderTarget*)params.resources->GetResource(params.layer->depth_stencil_target);
		Assert(depth_target);
		Assert(depth_target->GetDesc().width == depth_target->GetDesc().height);

		_shadow_map_size = depth_target->GetDesc().width;

		// No light parameters => Nothing to render
		if (!params.shader_params->HasVariable(_light_direction_variable))
			return;

		Vec3f light_direction = params.shader_params->GetVector3(_light_direction_variable).GetNormalized();

		{
			float near_range = params.camera->GetNearRange();
			float far_range = params.camera->GetFarRange();
			float split_constant = 0.45f;

			_slices[0].depth_begin = 0.0f;
			_slices[SHADOW_MAPPING_SLICE_COUNT - 1].depth_end = far_range;

			for (uint32_t i = 1; i < SHADOW_MAPPING_SLICE_COUNT; ++i)
			{
				float f = float(i) / (float)SHADOW_MAPPING_SLICE_COUNT;
				float log_distance = near_range * pow(far_range / near_range, f);
				float uniform_distance = near_range + (far_range - near_range) * f;

				_slices[i].depth_begin = (log_distance)+(1 - split_constant) * (uniform_distance);
				_slices[i - 1].depth_end = _slices[i].depth_begin;
			}

			shadow_mapping::UpdateSlicesJobData update_job_data;
			update_job_data.camera_world_matrix = params.camera->GetWorldMatrix();

			Vec3f frustum_corners[8];
			params.camera->CalculateFrustumCorners(frustum_corners);

			update_job_data.frustum_corners = frustum_corners;
			update_job_data.light_direction = light_direction;
			update_job_data.shadow_map_size = (float)_shadow_map_size;
			update_job_data.slices = _slices.data();

			scheduling::ParallelFor(params.scheduler, shadow_mapping::UpdateSlicesKernel, &update_job_data, Range(0, (uint32_t)_slices.size()));
		}

		Viewport vp;
		vp.x = 0;
		vp.y = 0;
		vp.min_depth = 0.0f;
		vp.max_depth = 1.0f;
		vp.width = (float)_shadow_map_size;
		vp.height = (float)_shadow_map_size;

		params.layer->Bind(render_context, params.resources, vp);

		RenderSlices(sort_key, params, render_context);



		// Calculate and bind any parameters that are needed for rendering the shadow map to the scene

		Mat4x4f global_shadow_matrix = shadow_mapping::CreateGlobalShadowMatrix(params.camera, light_direction);

		Mat4x4f shadow_view_proj[SHADOW_MAPPING_SLICE_COUNT];
		Vec3f cascade_offsets[SHADOW_MAPPING_SLICE_COUNT];
		Vec3f cascade_scales[SHADOW_MAPPING_SLICE_COUNT];

		for (uint32_t slice_index = 0; slice_index < _slices.size(); ++slice_index)
		{
			const shadow_mapping::ShadowSlice& slice = _slices[slice_index];
			shadow_mapping::CalculateCascadeOffsetAndScale(slice.shadow_view, slice.shadow_projection, global_shadow_matrix,
				cascade_offsets[slice_index], cascade_scales[slice_index]);
		}

		params.shader_params->SetMatrix4x4("shadow_matrix", global_shadow_matrix);

		float shadow_map_size = (float)_shadow_map_size;
		params.shader_params->SetVector2("min_max_border_padding", Vec2f(
			3.0f / shadow_map_size,
			(shadow_map_size - 3.0f) / shadow_map_size));

		params.shader_params->SetVector3Array("cascade_offsets", cascade_offsets, (uint32_t)_slices.size());
		params.shader_params->SetVector3Array("cascade_scales", cascade_scales, (uint32_t)_slices.size());
	}
	void ShadowMappingView::RenderSlices(uint64_t sort_key, const Params& params, RenderContext* render_context)
	{
		// Set up shader parameters
		Mat4x4f light_view_proj[SHADOW_MAPPING_SLICE_COUNT]; // TODO: Temp Alloc

		for (uint32_t slice_index = 0; slice_index < SHADOW_MAPPING_SLICE_COUNT; ++slice_index)
		{
			light_view_proj[slice_index] = _slices[slice_index].shadow_projection * _slices[slice_index].shadow_view;
		}

		params.shader_params->SetScalar("first_slice", (float)0);
		params.shader_params->SetScalar("last_slice", (float)SHADOW_MAPPING_SLICE_COUNT);

		params.shader_params->SetMatrix4x4Array("light_view_projection", light_view_proj, SHADOW_MAPPING_SLICE_COUNT);

		Camera shadow_camera;
		for (uint32_t slice_index = 0; slice_index < SHADOW_MAPPING_SLICE_COUNT; ++slice_index)
		{
			shadow_mapping::ShadowSlice& slice = _slices[slice_index];

			shadow_camera.SetProjectionMatrix(slice.shadow_projection);
			shadow_camera.SetViewMatrix(slice.shadow_view);
			shadow_camera.Update();

			params.shader_params->SetScalar("slice_index", (float)slice_index);


			// TODO: Culling

			for (auto& object : params.world->GetObjects())
			{
				RenderComponent::Type object_type = object->GetRenderType();

				switch (object_type)
				{
				case RenderComponent::MESH:
					((MeshComponent*)object)->Render(render_context, params.shader_params, params.camera, params.layer, sort_key);

				default:
					break;
				};
			}

		}

	}

} // namespace sb


