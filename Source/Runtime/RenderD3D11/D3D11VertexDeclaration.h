// Copyright 2008-2014 Simon Ekström

#ifndef __D3D11VERTEXDECLARATION_H__
#define __D3D11VERTEXDECLARATION_H__


namespace sb
{

	template<typename T>
	class D3D11ShaderProgram;

	struct D3D11VertexDeclaration
	{
		vector<D3D11_INPUT_ELEMENT_DESC> elements;
		map<D3D11ShaderProgram<ID3D11VertexShader>*, ID3D11InputLayout*> input_layouts;


		D3D11VertexDeclaration() {}
		~D3D11VertexDeclaration() {}
	};

} // namespace sb

#endif // __D3D11VERTEXDECLARATION_H__
