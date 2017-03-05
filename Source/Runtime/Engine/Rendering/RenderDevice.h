// Copyright 2008-2014 Simon Ekström

#ifndef __ENGINE_RENDERDEVICE_H__
#define __ENGINE_RENDERDEVICE_H__

namespace sb
{

	struct DisplayFormat
	{
		int width;
		int height;
	};

	class Window;
	class RenderContext;
	class RenderResource;
	class RRenderTarget;
	class RenderResourceAllocator;

	class RenderDevice
	{
	public:
		static const uint32_t INVALID_HANDLE = 0xffffffffu;

		struct InitParams
		{
			bool vsync;
			uint32_t adapter_index;

			InitParams() : vsync(false), adapter_index(0)
			{
			}
		};

	public:
		RenderDevice();
		virtual ~RenderDevice();

		virtual void Initialize(const InitParams& params) = 0;
		virtual void Shutdown() = 0;

		virtual uint32_t CreateSwapChain(Window* window, bool windowed_mode) = 0;
		virtual void ReleaseSwapChain(uint32_t swap_chain) = 0;

		void SetActiveSwapChain(uint32_t swap_chain);


		/// @brief Presents the specified swap chain
		virtual void Present() = 0;

		/// @brief Translates and dispatches render commands to the graphics API 
		virtual void Dispatch(uint32_t count, RenderContext** contexts) = 0;

		/// @brief Flushes any queued render resource allocations.
		virtual void FlushAllocator() = 0;

		//-------------------------------------------------------------------------------
		/// @brief Fills an array with available display formats
		///	@return Number of modes
		virtual uint32_t EnumDisplayFormats(uint32_t max_modes, DisplayFormat* modes) = 0;
		/// @brief Returns the desktop display format
		virtual DisplayFormat GetDesktopFormat() = 0;

		//-------------------------------------------------------------------------------

		/// @brief Creates a render context for rendering
		///	@sa ReleaseRenderContext
		RenderContext* CreateRenderContext();

		/// @brief Releases a render context created by CreateRenderContext
		///	@sa CreateRenderContext
		void ReleaseRenderContext(RenderContext* context);

		/// @brief Returns the active back buffer
		virtual RRenderTarget* GetBackBuffer() = 0;

		RenderResourceAllocator* GetResourceAllocator();


	protected:
		uint32_t _active_swap_chain;

		RenderResourceAllocator* _resource_allocator;
		MemoryPool<RenderContext, 16> _render_context_pool;
		CriticalSection _context_lock;


	};

} // namespace sb



#endif // __ENGINE_RENDERDEVICE_H__
