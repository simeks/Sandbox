// Copyright 2008-2014 Simon Ekström


namespace sb
{

	template<typename T>
	D3D11ShaderProgram<T>::D3D11ShaderProgram(T* shader, vector<uint8_t>* byte_code)
		: _shader(shader), _byte_code(byte_code)
	{
	}
	template<typename T>
	D3D11ShaderProgram<T>::~D3D11ShaderProgram()
	{
	}

	template<typename T>
	T* D3D11ShaderProgram<T>::GetShader()
	{
		return _shader;
	}
	template<typename T>
	vector<uint8_t>* D3D11ShaderProgram<T>::GetByteCode()
	{
		return _byte_code;
	}

} // namespace sb



