// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERD3D11_PCH_H__
#define __RENDERD3D11_PCH_H__

#include <Foundation/Common.h>
#include <Foundation/Memory/ComPtr.h>

#include "D3D11Rendering.h"

#define D3D_VERIFY( _exp ) \
	{ \
		HRESULT _hr = _exp; \
		if(FAILED(_hr))	\
		{ \
			logging::AssertMessage("%s:%d : Assertion: " #_exp " (0x%x)", __FILE__, __LINE__, _hr); \
			DEBUG_BREAK; \
		} \
	}






#endif // __RENDERD3D11_PCH_H__
