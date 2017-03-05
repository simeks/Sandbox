// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_PERFRAMEVARIABLEBINDER_H__
#define __RENDERING_PERFRAMEVARIABLEBINDER_H__


#include <Foundation/Math/Matrix4x4.h>
#include <Foundation/Math/Vec2.h>

namespace sb
{

	struct ConstantBufferReflection;

	struct ShaderPerFrameData
	{
		Mat4x4f camera_world;
		Mat4x4f camera_view;
		Mat4x4f camera_projection;
		Vec2f camera_near_far;

		Vec2f back_buffer_size;
	};


	/// Helper class for binding per frame shader variables. Mainly used for global constant buffers
	class PerFrameVariableBinder
	{
	public:
		PerFrameVariableBinder(const ConstantBufferReflection& refl);
		~PerFrameVariableBinder();

		/// Updates the specified constant buffer from the given per frame data
		void Bind(void* dest_data, const ShaderPerFrameData& per_frame_data);

	private:
		enum Type
		{
			CAMERA_WORLD,
			CAMERA_VIEW,
			CAMERA_VIEW_PROJECTION,
			CAMERA_PROJECTION,
			CAMERA_NEAR_FAR,
			BACK_BUFFER_SIZE,
			UNKNOWN
		};

		struct Binder
		{
			Type type;
			uint32_t offset;

		};

		map<StringId32, Binder> _binders;

	};

} // namespace sb

#endif // __RENDERING_PERFRAMEVARIABLEBINDER_H__
