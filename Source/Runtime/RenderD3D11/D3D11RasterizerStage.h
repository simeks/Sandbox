// Copyright 2008-2014 Simon Ekström

#ifndef __D3D11RASTERIZERSTAGE_H__
#define __D3D11RASTERIZERSTAGE_H__


namespace sb
{

	class D3D11DeviceContext;
	class D3D11RasterizerState;
	class D3D11RasterizerStage
	{
	public:
		D3D11RasterizerStage(D3D11DeviceContext* device_context);
		~D3D11RasterizerStage();

		void SetRasterizerState(D3D11RasterizerState* state);
		void SetViewports(uint32_t num, const D3D11_VIEWPORT* viewports);
		void SetScissorRects(uint32_t num, const D3D11_RECT* rects);

		void Apply();

		/// @return True if the state is dirty
		bool Dirty();

		/// @brief Resets the whole state
		void Clear();

	private:
		D3D11DeviceContext* _device_context;

		D3D11_VIEWPORT _viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
		uint32_t _num_viewports;

		D3D11_RECT _scissor_rects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
		uint32_t _num_rects;

		ID3D11RasterizerState* _state;
		bool _dirty;
	};

} // namespace sb

#endif // __D3D11RASTERIZERSTAGE_H__

