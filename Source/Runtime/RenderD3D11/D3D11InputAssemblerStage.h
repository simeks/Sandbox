// Copyright 2008-2014 Simon Ekström

#ifndef __D3D11INPUTASSEMBLERSTAGE_H__
#define __D3D11INPUTASSEMBLERSTAGE_H__

#include <Engine/Rendering/RenderBlock.h>


namespace sb
{

	template<typename T>
	class D3D11ShaderProgram;

	class D3D11DeviceContext;
	class D3D11InputAssemblerStage
	{
	public:
		D3D11InputAssemblerStage(D3D11DeviceContext* device_context);
		~D3D11InputAssemblerStage();

		void SetVertexBuffers(uint32_t slot, RenderResource* buffer, uint32_t offset);
		void SetIndexBuffer(RenderResource* buffer, uint32_t offset);

		void SetPrimitiveType(DrawCall::PrimitiveType type);

		void SetVertexDeclaration(RenderResource* declaration);
		void SetVertexProgram(D3D11ShaderProgram<ID3D11VertexShader>* vertex_program);

		void Apply();

		/// @return True if the state is dirty
		bool Dirty();

		/// @brief Resets the whole state
		void Clear();


		uint32_t GetVertexBuffer(uint32_t slot);
		uint32_t GetIndexBuffer();

	private:
		D3D11DeviceContext* _device_context;

		uint32_t _vb_handles[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
		uint32_t _vb_offsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

		uint32_t _ib_handle;
		uint32_t _ib_offset;

		DrawCall::PrimitiveType _prim_type;

		D3D11ShaderProgram<ID3D11VertexShader>* _vertex_program;
		uint32_t _vd_handle;


		bool _dirty;
	};

} // namespace sb



#endif // __D3D11INPUTASSEMBLERSTAGE_H__
