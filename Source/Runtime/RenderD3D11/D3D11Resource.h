// Copyright 2008-2014 Simon Ekström

#ifndef __D3D11RESOURCE_H__
#define __D3D11RESOURCE_H__


namespace sb
{

	struct D3D11Resource
	{
		virtual ~D3D11Resource() {}

		/// @brief Resource types for the renderer
		enum Type
		{
			BUFFER,
			TEXTURE1D,
			TEXTURE2D,
			TEXTURE3D
		};

		Type type;
	};

} // namespace sb



#endif // __D3D11RESOURCES_H__
