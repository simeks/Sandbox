// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "RenderState.h"

namespace sb
{

	bool render_state::ParseBool(const char* value)
	{
		if (strcmp(value, "true") == 0)
		{
			return true;
		}
		else if (strcmp(value, "false") == 0)
		{
			return false;
		}
		else
		{
			logging::Warning("Invalid variable \"%s\", expecting \"true\" or \"false\", defaulting to false.", value);
			return false;
		}
	}

	D3D11_BLEND render_state::ParseBlend(const char* value)
	{
		if (strcmp(value, "zero") == 0)
		{
			return D3D11_BLEND_ZERO;
		}
		else if (strcmp(value, "one") == 0)
		{
			return D3D11_BLEND_ONE;
		}
		else if (strcmp(value, "src_color") == 0)
		{
			return D3D11_BLEND_SRC_COLOR;
		}
		else if (strcmp(value, "dest_color") == 0)
		{
			return D3D11_BLEND_DEST_COLOR;
		}
		else if (strcmp(value, "src_alpha") == 0)
		{
			return D3D11_BLEND_SRC_ALPHA;
		}
		else if (strcmp(value, "dest_alpha") == 0)
		{
			return D3D11_BLEND_DEST_ALPHA;
		}
		else if (strcmp(value, "inv_src_color") == 0)
		{
			return D3D11_BLEND_INV_SRC_COLOR;
		}
		else if (strcmp(value, "inv_dest_color") == 0)
		{
			return D3D11_BLEND_INV_DEST_COLOR;
		}
		else if (strcmp(value, "inv_src_alpha") == 0)
		{
			return D3D11_BLEND_INV_SRC_ALPHA;
		}
		else if (strcmp(value, "inv_dest_alpha") == 0)
		{
			return D3D11_BLEND_INV_DEST_ALPHA;
		}
		else
		{
			logging::Warning("Invalid variable \"%s\", defaulting to \"zero\".", value);
			return D3D11_BLEND_ZERO;
		}
	}

	D3D11_BLEND_OP render_state::ParseBlendOp(const char* value)
	{
		if (strcmp(value, "add") == 0)
		{
			return D3D11_BLEND_OP_ADD;
		}
		else if (strcmp(value, "subtract") == 0)
		{
			return D3D11_BLEND_OP_SUBTRACT;
		}
		else if (strcmp(value, "min") == 0)
		{
			return D3D11_BLEND_OP_MIN;
		}
		else if (strcmp(value, "max") == 0)
		{
			return D3D11_BLEND_OP_MAX;
		}
		else
		{
			logging::Warning("Invalid variable \"%s\", defaulting to \"add\".", value);
			return D3D11_BLEND_OP_ADD;
		}
	}

	D3D11_COMPARISON_FUNC render_state::ParseCompareFunc(const char* value)
	{
		if (strcmp(value, "never") == 0)
		{
			return D3D11_COMPARISON_NEVER;
		}
		else if (strcmp(value, "less") == 0)
		{
			return D3D11_COMPARISON_LESS;
		}
		else if (strcmp(value, "equal") == 0)
		{
			return D3D11_COMPARISON_EQUAL;
		}
		else if (strcmp(value, "less_equal") == 0)
		{
			return D3D11_COMPARISON_LESS_EQUAL;
		}
		else if (strcmp(value, "greater") == 0)
		{
			return D3D11_COMPARISON_GREATER;
		}
		else if (strcmp(value, "not_equal") == 0)
		{
			return D3D11_COMPARISON_NOT_EQUAL;
		}
		else if (strcmp(value, "greater_equal") == 0)
		{
			return D3D11_COMPARISON_GREATER_EQUAL;
		}
		else if (strcmp(value, "always") == 0)
		{
			return D3D11_COMPARISON_ALWAYS;
		}
		else
		{
			logging::Warning("Invalid variable \"%s\", defaulting to \"never\".", value);
			return D3D11_COMPARISON_NEVER;
		}
	}

	D3D11_STENCIL_OP render_state::ParseStencilOp(const char* value)
	{
		if (strcmp(value, "keep") == 0)
		{
			return D3D11_STENCIL_OP_KEEP;
		}
		else if (strcmp(value, "zero") == 0)
		{
			return D3D11_STENCIL_OP_ZERO;
		}
		else if (strcmp(value, "replace") == 0)
		{
			return D3D11_STENCIL_OP_REPLACE;
		}
		else if (strcmp(value, "increment") == 0)
		{
			return D3D11_STENCIL_OP_INCR;
		}
		else if (strcmp(value, "decrement") == 0)
		{
			return D3D11_STENCIL_OP_DECR;
		}
		else if (strcmp(value, "increment_clamp") == 0)
		{
			return D3D11_STENCIL_OP_INCR_SAT;
		}
		else if (strcmp(value, "decrement_clamp") == 0)
		{
			return D3D11_STENCIL_OP_DECR_SAT;
		}
		else if (strcmp(value, "invert") == 0)
		{
			return D3D11_STENCIL_OP_INVERT;
		}
		else
		{
			logging::Warning("Invalid variable \"%s\", defaulting to \"keep\".", value);
			return D3D11_STENCIL_OP_KEEP;
		}
	}

	UINT8 render_state::ParseMask8(const char* value)
	{
		return (UINT8)strtoul(value, NULL, 16);
	}

	D3D11_FILL_MODE render_state::ParseFillMode(const char* value)
	{
		if (strcmp(value, "wireframe") == 0)
		{
			return D3D11_FILL_WIREFRAME;
		}
		else if (strcmp(value, "solid") == 0)
		{
			return D3D11_FILL_SOLID;
		}
		else
		{
			logging::Warning("Invalid variable \"%s\", defaulting to \"solid\".", value);
			return D3D11_FILL_SOLID;
		}
	}
	D3D11_CULL_MODE render_state::ParseCullMode(const char* value)
	{
		if (strcmp(value, "ccw") == 0)
		{
			return D3D11_CULL_BACK;
		}
		else if (strcmp(value, "cw") == 0)
		{
			return D3D11_CULL_FRONT;
		}
		else if (strcmp(value, "none") == 0)
		{
			return D3D11_CULL_NONE;
		}
		else
		{
			logging::Warning("Invalid variable \"%s\", defaulting to \"front\".", value);
			return D3D11_CULL_FRONT;
		}
	}

	D3D11_FILTER render_state::ParseFilter(const char* value)
	{
		if (strcmp(value, "point") == 0)
		{
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
		}
		else if (strcmp(value, "linear") == 0)
		{
			return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		}
		else if (strcmp(value, "anisotropic") == 0)
		{
			return D3D11_FILTER_ANISOTROPIC;
		}
		else if (strcmp(value, "comparison_point") == 0)
		{
			return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		}
		else if (strcmp(value, "comparison_linear") == 0)
		{
			return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		}
		else
		{
			logging::Warning("Invalid variable \"%s\", defaulting to \"linear\".", value);
			return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		}
	}
	D3D11_TEXTURE_ADDRESS_MODE render_state::ParseTAM(const char* value)
	{
		if (strcmp(value, "wrap") == 0)
		{
			return D3D11_TEXTURE_ADDRESS_WRAP;
		}
		else if (strcmp(value, "mirror") == 0)
		{
			return D3D11_TEXTURE_ADDRESS_MIRROR;
		}
		else if (strcmp(value, "clamp") == 0)
		{
			return D3D11_TEXTURE_ADDRESS_CLAMP;
		}
		else if (strcmp(value, "border") == 0)
		{
			return D3D11_TEXTURE_ADDRESS_BORDER;
		}
		else
		{
			logging::Warning("Invalid variable \"%s\", defaulting to \"clamp\".", value);
			return D3D11_TEXTURE_ADDRESS_CLAMP;
		}
	}

} // namespace sb
