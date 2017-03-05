// Copyright 2008-2014 Simon Ekström

#ifndef __D3D11RENDERSTATES_H__
#define __D3D11RENDERSTATES_H__


namespace sb
{

	class D3D11ResourceManager;
	class D3D11BlendState
	{
	public:
		D3D11BlendState();
		~D3D11BlendState();

		void Create(D3D11ResourceManager* resource_manager, const D3D11_BLEND_DESC& desc, float* blend_factor, uint32_t sample_mask);

		ID3D11BlendState* GetState() const;
		float const* GetBlendFactor() const;
		uint32_t GetSampleMask() const;

	private:
		/// Sets to the default state
		void SetDefault();

		D3D11_BLEND_DESC _desc;
		float _blend_factor[4];
		uint32_t _sample_mask;

		ID3D11BlendState* _state;

	};

	class D3D11DepthStencilState
	{
	public:
		D3D11DepthStencilState();
		~D3D11DepthStencilState();

		void Create(D3D11ResourceManager* resource_manager, const D3D11_DEPTH_STENCIL_DESC& desc, uint32_t stencil_ref);

		ID3D11DepthStencilState* GetState() const;
		uint32_t GetStencilRef() const;

	private:
		/// Sets to the default state
		void SetDefault();

		D3D11_DEPTH_STENCIL_DESC _desc;
		uint32_t _stencil_ref;

		ID3D11DepthStencilState* _state;

	};

	class D3D11RasterizerState
	{
	public:
		D3D11RasterizerState();
		~D3D11RasterizerState();

		void Create(D3D11ResourceManager* resource_manager, const D3D11_RASTERIZER_DESC& desc);

		ID3D11RasterizerState* GetState() const;

	private:
		/// Sets to the default state
		void SetDefault();

		D3D11_RASTERIZER_DESC _desc;
		ID3D11RasterizerState* _state;

	};

	class D3D11SamplerState
	{
	public:
		D3D11SamplerState();
		~D3D11SamplerState();

		void Create(D3D11ResourceManager* resource_manager, const D3D11_SAMPLER_DESC& desc);

		ID3D11SamplerState* GetState() const;

	private:
		/// Sets to the default state
		void SetDefault();

		D3D11_SAMPLER_DESC _desc;
		ID3D11SamplerState* _state;

	};

} // namespace sb




#endif // __D3D11RENDERSTATES_H__
