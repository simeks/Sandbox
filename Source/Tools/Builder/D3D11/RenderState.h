// Copyright 2008-2014 Simon Ekström

#ifndef __BUILDER_D3D11_RENDERSTATE_H__
#define __BUILDER_D3D11_RENDERSTATE_H__

#include <d3d11.h>

namespace sb
{

	namespace render_state
	{
		bool ParseBool(const char* value);

		D3D11_BLEND ParseBlend(const char* value);

		D3D11_BLEND_OP ParseBlendOp(const char* value);

		D3D11_COMPARISON_FUNC ParseCompareFunc(const char* value);

		D3D11_STENCIL_OP ParseStencilOp(const char* value);

		UINT8 ParseMask8(const char* value);

		D3D11_FILL_MODE ParseFillMode(const char* value);
		D3D11_CULL_MODE ParseCullMode(const char* value);

		D3D11_FILTER ParseFilter(const char* value);
		D3D11_TEXTURE_ADDRESS_MODE ParseTAM(const char* value);

	};

} // namespace sb


#endif // __BUILDER_D3D11_RENDERSTATE_H__
