// Copyright 2008-2014 Simon Ekström

#ifndef __D3D11RENDERDEVICE_H__
#define __D3D11RENDERDEVICE_H__

#include <Engine/Rendering/RenderDevice.h>
#include <Engine/Rendering/RenderContext.h>


namespace sb
{

	class RRenderTarget;

	class D3D11DeviceContext;
	class D3D11ResourceManager;
	struct D3D11MemoryStatistics;

	class D3D11RenderDevice : public RenderDevice
	{
	public:
		D3D11RenderDevice();
		~D3D11RenderDevice();

		void Initialize(const InitParams& params);
		void Shutdown();

		uint32_t CreateSwapChain(Window* window, bool windowed_mode = true);
		void ReleaseSwapChain(uint32_t swap_chain_handle);

		void Present();

		void Dispatch(uint32_t count, RenderContext** context);
		void FlushAllocator();

		//-------------------------------------------------------------------------------
		/// @brief Fills an array with available display formats
		///	@return Number of modes
		uint32_t EnumDisplayFormats(uint32_t max_modes, DisplayFormat* modes);
		/// @brief Returns the desktop display format
		DisplayFormat GetDesktopFormat();

		//-------------------------------------------------------------------------------

		RRenderTarget* GetBackBuffer();

		//-------------------------------------------------------------------------------

		ID3D11Device* GetD3DDevice() const;
		ID3D11DeviceContext* GetD3DImmediateContext() const;
		D3D11ResourceManager* GetD3D11ResourceManager() const;
		//-------------------------------------------------------------------------------

		D3D11MemoryStatistics* GetMemoryStatistics();

	private:

		struct SwapChain
		{
			Window* window;
			ComPtr<IDXGISwapChain> swap_chain;
			RRenderTarget* back_buffer;

			bool used; ///< False if this swap chain is released and available for re-use
		};

		ComPtr<ID3D11Device> _device;
		ComPtr<ID3D11Debug> _debug;
		ComPtr<ID3D11DeviceContext> _d3d_imm_context;
		D3D11DeviceContext* _imm_context;

		D3D11ResourceManager* _resource_manager;

		vector<SwapChain> _swap_chains;

		InitParams _device_params;

		RenderContext::SortCmdList _sort_cmds; // Temporary arrays for holding merged sort commands
	};

} // namespace sb



#endif // __D3D11RENDERDEVICE_H__
