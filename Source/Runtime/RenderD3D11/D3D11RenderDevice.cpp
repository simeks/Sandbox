// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "D3D11RenderDevice.h"
#include "D3D11DeviceContext.h"
#include "D3D11ResourceManager.h"
#include <Engine/Rendering/RRenderTarget.h>
#include <Foundation/Resource/ResourceManager.h>
#include <Foundation/Profiler/Profiler.h>

#include <Framework/Window/Window.h>


namespace sb
{

	//-------------------------------------------------------------------------------
	D3D11RenderDevice::D3D11RenderDevice()
		: _device(nullptr),
		_debug(nullptr),
		_d3d_imm_context(nullptr),
		_imm_context(nullptr),
		_resource_manager(nullptr)
	{
	}
	D3D11RenderDevice::~D3D11RenderDevice()
	{
	}
	//-------------------------------------------------------------------------------
	void D3D11RenderDevice::Initialize(const InitParams& params)
	{
		HRESULT hr;
		UINT device_flags = 0;

		IDXGIFactory1* dxgi_factory = 0;
		D3D_VERIFY(CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)(&dxgi_factory)));

		// Enumerate all the adapters in the system
		IDXGIAdapter1* adapter;
		for (uint32_t i = 0;; ++i)
		{
			hr = dxgi_factory->EnumAdapters1(i, &adapter);
			if (hr == DXGI_ERROR_NOT_FOUND)
				break;

			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);
			adapter->Release();
			logging::Info("D3D11Adapter : %ls", desc.Description);
		}

		SAFE_RELEASE(dxgi_factory);

		// If we debug use device debug
#ifdef SANDBOX_BUILD_DEBUG
		device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		// MT/Non-MT etc... TODO:
		// device_flags |= D3D11_CREATE_DEVICE_SINGLETHREADED;

		D3D_FEATURE_LEVEL feature_level[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};

		D3D_VERIFY(D3D11CreateDevice(
			0, // Adapter TODO:
			D3D_DRIVER_TYPE_HARDWARE,
			0,
			device_flags,
			feature_level,
			6,
			D3D11_SDK_VERSION,
			&_device,
			0,
			&_d3d_imm_context
			));


#ifdef SANDBOX_BUILD_DEBUG
		// Retrieve D3D11 debug inteface
		D3D_VERIFY(_device->QueryInterface(__uuidof(ID3D11Debug), (void**)&_debug));

#endif

		// Create our resource manager
		_resource_manager = new D3D11ResourceManager(this);

		// Create device context object for our immediate context
		_imm_context = new D3D11DeviceContext(_d3d_imm_context.Get(), this, _resource_manager);

		_device_params = params;
	}
	void D3D11RenderDevice::Shutdown()
	{
		delete _imm_context;
		_imm_context = nullptr;

		delete _resource_manager;
		_resource_manager = nullptr;

	}
	//------------------------------------------------------------------------------
	uint32_t D3D11RenderDevice::CreateSwapChain(Window* window, bool windowed_mode)
	{
		ComPtr<IDXGISwapChain> dxgi_swap_chain;

		IDXGIDevice * dxgi_device;
		D3D_VERIFY(_device->QueryInterface(__uuidof(IDXGIDevice), (void **)&dxgi_device));

		IDXGIAdapter * dxgi_adapter;
		D3D_VERIFY(dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void **)&dxgi_adapter));

		IDXGIFactory * dxgi_factory;
		D3D_VERIFY(dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void **)&dxgi_factory));

		DXGI_SWAP_CHAIN_DESC swap_desc;
		ZeroMemory(&swap_desc, sizeof(DXGI_SWAP_CHAIN_DESC));

		swap_desc.Windowed = windowed_mode;
		swap_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swap_desc.BufferCount = _device_params.vsync ? 2 : 1;
		swap_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap_desc.OutputWindow = window->_hwnd;
		swap_desc.BufferDesc.Width = window->_width;
		swap_desc.BufferDesc.Height = window->_height;

		if (_device_params.vsync)
		{
			swap_desc.BufferDesc.RefreshRate.Numerator = 60;
			swap_desc.BufferDesc.RefreshRate.Denominator = 1;
		}
		else
		{
			swap_desc.BufferDesc.RefreshRate.Numerator = 0;
			swap_desc.BufferDesc.RefreshRate.Denominator = 1;
		}

		swap_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		swap_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swap_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		// TODO: FSAA
		swap_desc.SampleDesc.Count = 1;
		swap_desc.SampleDesc.Quality = 0;

		D3D_VERIFY(dxgi_factory->CreateSwapChain(dxgi_device, &swap_desc, &dxgi_swap_chain));

		dxgi_factory->Release();
		dxgi_adapter->Release();
		dxgi_device->Release();


		uint32_t index = 0;

		// Check if we have any free slots in our array
		for (auto& swap_chain : _swap_chains)
		{
			if (!swap_chain.used)
			{
				swap_chain.swap_chain = dxgi_swap_chain;
				swap_chain.window = window;
				swap_chain.used = true;
				break;
			}
		}

		if (index == _swap_chains.size())
		{
			SwapChain swap_chain;
			swap_chain.swap_chain = dxgi_swap_chain;
			swap_chain.window = window;
			swap_chain.used = true;

			_swap_chains.push_back(swap_chain);
		}

		TextureDesc desc;
		desc.pixel_format = image::PF_R8G8B8A8;
		desc.type = TextureDesc::TYPE_2D;
		desc.width = window->_width;
		desc.height = window->_height;
		desc.mip_count = 1;
		desc.array_size = 1;

		// Retrieve the back buffer and create a render target for it
		RRenderTarget* back_buffer = new RRenderTarget(desc, RRenderTarget::BIND_RENDER_TARGET);
		_swap_chains[index].back_buffer = back_buffer;

		back_buffer->SetHandle(_resource_allocator->GetHandleGenerator().New());
		_resource_manager->AllocateBackBuffer(back_buffer->GetHandle(), desc, dxgi_swap_chain.Get());

		return index;
	}
	void D3D11RenderDevice::ReleaseSwapChain(uint32_t swap_chain_handle)
	{
		Assert(swap_chain_handle <= _swap_chains.size());

		SwapChain& swap_chain = _swap_chains[swap_chain_handle];
		Assert(swap_chain.used == true); // Check if swap chain is already released

		// Make sure to disable fullscreen for the swap chain if set.
		swap_chain.swap_chain->SetFullscreenState(FALSE, NULL);

		// Release render target
		_resource_manager->ReleaseResource(*swap_chain.back_buffer);
		_resource_allocator->GetHandleGenerator().Release(swap_chain.back_buffer->GetHandle());

		delete swap_chain.back_buffer;
		swap_chain.back_buffer = 0;

		swap_chain.swap_chain = nullptr;
		swap_chain.window = 0;
		swap_chain.used = false;

	}
	//-------------------------------------------------------------------------------
	namespace
	{
		bool SortCmdCompare(const RenderContext::SortCmd& l, const RenderContext::SortCmd& r)
		{
			return (l.sort_key < r.sort_key);
		}
	}
	void D3D11RenderDevice::Dispatch(uint32_t count, RenderContext** contexts)
	{
		// Flush any queued resource commands
		FlushAllocator();

		for (uint32_t i = 0; i < count; ++i)
		{
			const RenderContext::SortCmdList& cmd_list = contexts[i]->GetSortCmds();
			if (cmd_list.size())
			{
				_sort_cmds.insert(_sort_cmds.end(), cmd_list.begin(), cmd_list.end());
			}
		}

		{
			PROFILER_SCOPE("Command sort");
			std::stable_sort(_sort_cmds.data(), _sort_cmds.data() + _sort_cmds.size(), SortCmdCompare);
		}

		_imm_context->Dispatch((uint32_t)_sort_cmds.size(), _sort_cmds.data());
		_sort_cmds.clear();
	}
	void D3D11RenderDevice::FlushAllocator()
	{
		_resource_manager->FlushAllocator(_resource_allocator);
	}
	//-------------------------------------------------------------------------------
	void D3D11RenderDevice::Present()
	{
		Assert(_active_swap_chain <= _swap_chains.size());

		SwapChain& swap_chain = _swap_chains[_active_swap_chain];
		Assert(swap_chain.used == true);
		Assert(swap_chain.swap_chain);

		if (_device_params.vsync)
			swap_chain.swap_chain->Present(1, 0);
		else
			swap_chain.swap_chain->Present(0, 0);
	}
	//-------------------------------------------------------------------------------
	uint32_t D3D11RenderDevice::EnumDisplayFormats(uint32_t max_modes, DisplayFormat* modes)
	{
		Assert(_device.Get());

		IDXGIDevice*	dxgi_device;
		IDXGIAdapter*	dxgi_adapter;
		IDXGIOutput*	dxgi_output;

		DXGI_FORMAT		fmt = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		UINT			num_modes = 0;
		DXGI_MODE_DESC*	display_modes = NULL;

		D3D_VERIFY(_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgi_device));

		D3D_VERIFY(dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgi_adapter));

		dxgi_adapter->EnumOutputs(0, &dxgi_output);

		// Get number of elements
		dxgi_output->GetDisplayModeList(fmt, 0, &num_modes, NULL);
		display_modes = new DXGI_MODE_DESC[num_modes]; // TODO: Temp alloc

		// Get modes
		dxgi_output->GetDisplayModeList(fmt, 0, &num_modes, display_modes);

		// Release interfaces
		dxgi_output->Release();
		dxgi_adapter->Release();
		dxgi_device->Release();


		// Make sure we don't write more than 'maxModes' elements
		num_modes = Min(num_modes, max_modes);
		// Fill array
		for (uint32_t i = 0; i < num_modes; ++i)
		{
			modes[i].width = display_modes[i].Width;
			modes[i].height = display_modes[i].Height;
		}

		delete[] display_modes; // TODO: Temp alloc

		return num_modes;
	}
	DisplayFormat D3D11RenderDevice::GetDesktopFormat()
	{
		Assert(_device.Get());

		DisplayFormat display_fmt;
		display_fmt.width = display_fmt.height = 0;

		IDXGIDevice*	dxgi_device;
		IDXGIAdapter*	dxgi_adapter;
		IDXGIOutput*	dxgi_output;

		D3D_VERIFY(_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgi_device));

		D3D_VERIFY(dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgi_adapter));

		D3D_VERIFY(dxgi_adapter->EnumOutputs(0, &dxgi_output));

		// Get output description
		DXGI_OUTPUT_DESC desc;
		dxgi_output->GetDesc(&desc);

		display_fmt.width = desc.DesktopCoordinates.right - desc.DesktopCoordinates.left;
		display_fmt.height = desc.DesktopCoordinates.bottom - desc.DesktopCoordinates.top;

		// Release interfaces
		dxgi_output->Release();
		dxgi_adapter->Release();
		dxgi_device->Release();

		return display_fmt;
	}
	//------------------------------------------------------------------------------
	ID3D11Device* D3D11RenderDevice::GetD3DDevice() const
	{
		Assert(_device != nullptr);
		return _device.Get();
	}
	ID3D11DeviceContext* D3D11RenderDevice::GetD3DImmediateContext() const
	{
		Assert(_device != nullptr);
		return _d3d_imm_context.Get();
	}

	D3D11ResourceManager* D3D11RenderDevice::GetD3D11ResourceManager() const
	{
		Assert(_resource_manager);
		return _resource_manager;
	}
	//-------------------------------------------------------------------------------
	RRenderTarget* D3D11RenderDevice::GetBackBuffer()
	{
		Assert(_active_swap_chain <= _swap_chains.size());

		SwapChain& swap_chain = _swap_chains[_active_swap_chain];
		Assert(swap_chain.used == true);

		return swap_chain.back_buffer;
	}
	//-------------------------------------------------------------------------------
	D3D11MemoryStatistics* D3D11RenderDevice::GetMemoryStatistics()
	{
		Assert(_resource_manager);
		Assert(_resource_manager->GetMemoryStatistics());
		return _resource_manager->GetMemoryStatistics();
	}

	//-------------------------------------------------------------------------------

} // namespace sb


