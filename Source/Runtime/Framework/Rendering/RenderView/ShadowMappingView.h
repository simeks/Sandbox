// Copyright 2008-2014 Simon Ekström

#ifndef __FRAMEWORK_SHADOWMAPPINGVIEW_H__
#define __FRAMEWORK_SHADOWMAPPINGVIEW_H__

#include "RenderView.h"

#include <foundation/math/Matrix4x4.h>
#include <foundation/math/Vec3.h>

namespace sb
{
	class ConfigValue;
	class RenderResourceAllocator;
	class RenderResourceSet;

	namespace shadow_mapping
	{
		struct ShadowSlice
		{
			Mat4x4f shadow_projection;
			Mat4x4f shadow_view;
			float depth_begin;
			float depth_end;
		};

		Mat4x4f CreateGlobalShadowMatrix(const Camera* camera, const Vec3f& light_direction);
		void CalculateCascadeOffsetAndScale(const Mat4x4f& shadow_view_matrix, 
											const Mat4x4f& shadow_proj_matrix,
											const Mat4x4f& global_view_matrix, 
											Vec3f& out_offset, 
											Vec3f& out_scale);
	};

	class ShadowMappingView : public RenderView
	{
	public:
		ShadowMappingView();
		~ShadowMappingView();

		void Load(const ConfigValue& config, 
				  RenderResourceAllocator* resource_allocator, 
				  RenderResourceSet* resource_set) OVERRIDE;
		void Unload(RenderResourceAllocator* resource_allocator) OVERRIDE;

		void Render(uint64_t sort_key, const Params& params, RenderContext* render_context) OVERRIDE;

	private:
		void RenderSlices(uint64_t sort_key, const Params& params, RenderContext* render_context);

		vector<shadow_mapping::ShadowSlice> _slices;
		uint32_t _shadow_map_size;
		uint32_t _pcf_filter_size;

		StringId32 _light_direction_variable;

	};

} // namespace sb


#endif // __FRAMEWORK_SHADOWMAPPINGVIEW_H__
