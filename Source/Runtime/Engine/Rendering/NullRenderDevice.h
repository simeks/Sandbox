// Copyright 2008-2014 Simon Ekström

#ifndef __ENGINE_NULLRENDERDEVICE_H__
#define __ENGINE_NULLRENDERDEVICE_H__

#include "RenderDevice.h"
#include "HandleGenerator.h"

namespace sb
{

	class NullRenderDevice : public RenderDevice
	{
	public:
		NullRenderDevice();
		virtual ~NullRenderDevice();

		virtual void Initialize(const InitParams& params);
		virtual void Shutdown();

		virtual uint32_t CreateSwapChain(Window* window, bool windowed_mode);
		virtual void ReleaseSwapChain(uint32_t swap_chain);

		virtual void Present();

		/// @brief Translates and dispatches render commands to the graphics API 
		virtual void Dispatch(uint32_t count, RenderContext** contexts);

		/// @brief Flushes any queued render resource allocations.
		virtual void FlushAllocator();

		/// @brief Fills an array with available display formats
		///	@return Number of modes
		virtual uint32_t EnumDisplayFormats(uint32_t max_modes, DisplayFormat* modes);
		/// @brief Returns the desktop display format
		virtual DisplayFormat GetDesktopFormat();

		virtual RRenderTarget* GetBackBuffer();



	};

} // namespace sb



#endif // __ENGINE_NULLRENDERDEVICE_H__
