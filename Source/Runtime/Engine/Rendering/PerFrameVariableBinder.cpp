// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "PerFrameVariableBinder.h"
#include "Shader.h"

#include <Foundation/Math/MatrixUtil.h>

namespace sb
{

	PerFrameVariableBinder::PerFrameVariableBinder(const ConstantBufferReflection& refl)
	{
		for (auto& var : refl.variables)
		{
			Binder binder;
			binder.offset = var.offset;
			binder.type = UNKNOWN;

			if (var.name == "camera_world")
			{
				Assert(var.var_class == ShaderVariable::MATRIX4X4);
				binder.type = CAMERA_WORLD;
			}
			else if (var.name == "camera_view")
			{
				Assert(var.var_class == ShaderVariable::MATRIX4X4);
				binder.type = CAMERA_VIEW;
			}
			else if (var.name == "camera_view_projection")
			{
				Assert(var.var_class == ShaderVariable::MATRIX4X4);
				binder.type = CAMERA_VIEW_PROJECTION;
			}
			else if (var.name == "camera_projection")
			{
				Assert(var.var_class == ShaderVariable::MATRIX4X4);
				binder.type = CAMERA_PROJECTION;
			}
			else if (var.name == "camera_near_far")
			{
				Assert(var.var_class == ShaderVariable::VECTOR2);
				binder.type = CAMERA_NEAR_FAR;
			}
			else if (var.name == "back_buffer_size")
			{
				Assert(var.var_class == ShaderVariable::VECTOR2);
				binder.type = BACK_BUFFER_SIZE;
			}

			if (binder.type != UNKNOWN)
				_binders[var.name] = binder;
		}
	}
	PerFrameVariableBinder::~PerFrameVariableBinder()
	{
	}
	void PerFrameVariableBinder::Bind(void* dest_data, const ShaderPerFrameData& per_frame_data)
	{
		Mat4x4f view_proj = per_frame_data.camera_projection * per_frame_data.camera_view;

		uint8_t* dest = (uint8_t*)dest_data;

		for (auto& binder : _binders)
		{
			switch (binder.second.type)
			{
			case CAMERA_WORLD:
				memcpy(dest + binder.second.offset, &per_frame_data.camera_world, sizeof(Mat4x4f));
				break;
			case CAMERA_VIEW:
				memcpy(dest + binder.second.offset, &per_frame_data.camera_view, sizeof(Mat4x4f));
				break;
			case CAMERA_VIEW_PROJECTION:
				memcpy(dest + binder.second.offset, &view_proj, sizeof(Mat4x4f));
				break;
			case CAMERA_PROJECTION:
				memcpy(dest + binder.second.offset, &per_frame_data.camera_projection, sizeof(Mat4x4f));
				break;
			case CAMERA_NEAR_FAR:
				memcpy(dest + binder.second.offset, &per_frame_data.camera_near_far, sizeof(Vec2f));
				break;
			case BACK_BUFFER_SIZE:
				memcpy(dest + binder.second.offset, &per_frame_data.back_buffer_size, sizeof(Vec2f));
				break;
			case UNKNOWN:
				break;
			};
		}
	}

} // namespace sb
