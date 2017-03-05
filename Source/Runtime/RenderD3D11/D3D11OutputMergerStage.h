// Copyright 2008-2014 Simon Ekström

#ifndef __D3D11OUTPUTMERGERSTAGE_H__
#define __D3D11OUTPUTMERGERSTAGE_H__


namespace sb
{

	class RenderTarget;
	class RenderResource;
	class D3D11DeviceContext;
	class D3D11BlendState;
	class D3D11DepthStencilState;
	class D3D11ShaderPass;

	/// @brief Simple class for keeping track of the pipelines output merger state
	/// TODO: Should we move UAVs to the output merger and use "OMSetRenderTargetsAndUnorderedAccessViews"?
	class D3D11OutputMergerStage
	{
	public:
		D3D11OutputMergerStage(D3D11DeviceContext* device_context);
		~D3D11OutputMergerStage();

		void SetRenderTarget(uint32_t index, uint32_t render_target);
		void SetDepthStencilTarget(uint32_t depth_target);

		void SetBlendState(D3D11BlendState* state);
		void SetDepthStencilState(D3D11DepthStencilState* state);

		void BindUnorderedAccessViews(RenderResource* resources, uint32_t resource_count, const D3D11ShaderPass* shader_pass);

		/// Applies the output merger state for a compute shader stage
		///	This meaning that only UAVs will be bound, no render targets.
		void ApplyComputeStage();

		/// Applies the output merger state for a pixel shader stage
		void ApplyPixelStage();

		/// @return True if the state is dirty
		bool Dirty();

		/// @brief Resets the whole state
		void Clear();

		/// @brief Returns an array of size MAX_MULTIPLE_RENDER_TARGETS, with all bound render target views
		ID3D11RenderTargetView** GetRenderTargetViews();
		ID3D11DepthStencilView* GetDepthStencilView();

	private:
		D3D11DeviceContext* _device_context;

		uint32_t _render_targets[MAX_MULTIPLE_RENDER_TARGETS];
		uint32_t _depth_stencil_target;

		ID3D11RenderTargetView* _rtv[MAX_MULTIPLE_RENDER_TARGETS];
		ID3D11DepthStencilView* _dsv;
		ID3D11UnorderedAccessView* _render_target_uav[MAX_MULTIPLE_RENDER_TARGETS];

		ID3D11BlendState* _blend_state;
		float _blend_factor[4];
		uint32_t _sample_mask;

		ID3D11DepthStencilState* _depth_stencil_state;
		uint32_t _stencil_ref;

		vector<ID3D11UnorderedAccessView*> _uav;
		vector<uint32_t> _uav_initial_counts;
		uint32_t _uav_start_slot;
		uint32_t _uav_count;

		bool _target_dirty;
		bool _blend_state_dirty;
		bool _depth_stencil_state_dirty;
	};

} // namespace sb


#endif // __D3D11OUTPUTMERGERSTAGE_H__