// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_RENDERRESOURCE_H__
#define __RENDERING_RENDERRESOURCE_H__


namespace sb
{

	class RenderResource
	{
	public:
		enum Type
		{
			UNKNOWN = 0,

			TEXTURE = 1,
			RENDER_TARGET = 2,
			VERTEX_DECLARATION = 3,
			VERTEX_BUFFER = 4,
			INDEX_BUFFER = 5,
			CONSTANT_BUFFER = 6,
			SHADER = 7,
			RAW_BUFFER = 8 ///< Raw data buffer

		};

		RenderResource();
		RenderResource(Type type);
		RenderResource(Type type, uint32_t handle);


		void SetHandle(uint32_t handle);
		uint32_t GetHandle() const;

		Type GetType() const;

		/// @brief Returns true if this resource is currently allocated and valid for use. False if not.
		bool IsValid() const;

	private:
		uint32_t _handle;
		Type _type;
	};


} // namespace sb


#endif // __RENDERING_RENDERRESOURCE_H__

