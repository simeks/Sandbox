// Copyright 2008-2014 Simon Ekström

#ifndef __D3D11DEVICECONTEXT_H__
#define __D3D11DEVICECONTEXT_H__

#include "D3D11OutputMergerStage.h"
#include "D3D11InputAssemblerStage.h"
#include "D3D11RasterizerStage.h"
#include "D3D11ShaderStage.h"
#include "D3D11InstanceMerger.h"

#include <Engine/Rendering/RenderContext.h>



namespace sb
{

	class D3D11RenderDevice;
	class D3D11ResourceManager;
	class D3D11DeviceContext
	{
	public:
		D3D11DeviceContext(ID3D11DeviceContext* d3d_context,
			D3D11RenderDevice* device, D3D11ResourceManager* resource_manager);
		~D3D11DeviceContext();

		/// @brief Translates and dispatches RenderContext commands
		void Dispatch(uint32_t count, const RenderContext::SortCmd* commands);

		/// @brief Executes a render command.
		void Draw(RenderContext::DrawCmd* cmd, uint64_t sort_key);

		D3D11ResourceManager* GetResourceManager() const;
		ID3D11DeviceContext* GetD3DContext() const;

	private:

		/// @brief Dispatches a compute shader
		void DispatchCompute(RenderContext::DispatchCmd* cmd, uint64_t sort_key);


		/// Clears the current device context states
		void ClearState();

		ID3D11DeviceContext* _context;
		D3D11RenderDevice* _device;
		D3D11ResourceManager* _resource_manager;

		D3D11OutputMergerStage _output_merger_stage;
		D3D11InputAssemblerStage _input_assembler_stage;
		D3D11RasterizerStage _rasterizer_stage;

		D3D11ShaderStage _shader_stage;
		D3D11InstanceMerger _instance_merger;

	};

} // namespace sb



#endif // __D3D11DEVICECONTEXT_H__

