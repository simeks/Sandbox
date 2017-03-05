// Copyright 2008-2014 Simon Ekström

#ifndef __D3D11SHADERPROGRAM_H__
#define __D3D11SHADERPROGRAM_H__


namespace sb
{

	class D3D11RenderDevice;

	template<typename T>
	class D3D11ShaderProgram : NonCopyable
	{
	public:
		D3D11ShaderProgram(T* shader, vector<uint8_t>* byte_code);
		~D3D11ShaderProgram();

		T* GetShader();
		vector<uint8_t>* GetByteCode();

	private:
		vector<uint8_t>* _byte_code;
		T* _shader;
	};

} // namespace sb

#include "D3D11ShaderProgram.inl"


#endif // __D3D11SHADERPROGRAM_H__
