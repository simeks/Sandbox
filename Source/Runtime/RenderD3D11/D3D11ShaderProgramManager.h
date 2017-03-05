// Copyright 2008-2014 Simon Ekström

#ifndef __D3D11SHADERPROGRAMMANAGER_H__
#define __D3D11SHADERPROGRAMMANAGER_H__

namespace sb
{

	class D3D11RenderDevice;
	class D3D11ShaderProgramManager
	{
	public:
		D3D11ShaderProgramManager(D3D11RenderDevice* render_device);
		~D3D11ShaderProgramManager();

		/// @brief Returns a shader object for the specified bytecode
		/// This method will first check if a shader object matching the bytecode already exists,
		///		if not it will just create a new one.
		ID3D11VertexShader* GetVertexShader(const vector<uint8_t>& byte_code);

		/// @brief Returns a shader object for the specified bytecode
		/// This method will first check if a shader object matching the bytecode already exists,
		///		if not it will just create a new one.
		ID3D11HullShader* GetHullShader(const vector<uint8_t>& byte_code);

		/// @brief Returns a shader object for the specified bytecode
		/// This method will first check if a shader object matching the bytecode already exists,
		///		if not it will just create a new one.
		ID3D11DomainShader* GetDomainShader(const vector<uint8_t>& byte_code);

		/// @brief Returns a shader object for the specified bytecode
		/// This method will first check if a shader object matching the bytecode already exists,
		///		if not it will just create a new one.
		ID3D11GeometryShader* GetGeometryShader(const vector<uint8_t>& byte_code);

		/// @brief Returns a shader object for the specified bytecode
		/// This method will first check if a shader object matching the bytecode already exists,
		///		if not it will just create a new one.
		ID3D11PixelShader* GetPixelShader(const vector<uint8_t>& byte_code);

		/// @brief Returns a shader object for the specified bytecode
		/// This method will first check if a shader object matching the bytecode already exists,
		///		if not it will just create a new one.
		ID3D11ComputeShader* GetComputeShader(const vector<uint8_t>& byte_code);


		/// Templated method for retrieving a shader object from a specified bytecode
		template<typename T> T* GetShader(const vector<uint8_t>& byte_code);



		/// @brief Releases the specified shader, destroying it if the reference count hits 0
		void ReleaseShader(ID3D11VertexShader* shader);

		/// @brief Releases the specified shader, destroying it if the reference count hits 0
		void ReleaseShader(ID3D11HullShader* shader);

		/// @brief Releases the specified shader, destroying it if the reference count hits 0
		void ReleaseShader(ID3D11DomainShader* shader);

		/// @brief Releases the specified shader, destroying it if the reference count hits 0
		void ReleaseShader(ID3D11GeometryShader* shader);

		/// @brief Releases the specified shader, destroying it if the reference count hits 0
		void ReleaseShader(ID3D11PixelShader* shader);

		/// @brief Releases the specified shader, destroying it if the reference count hits 0
		void ReleaseShader(ID3D11ComputeShader* shader);

	private:
		template<typename T>
		struct ProgramEntry
		{
			T* shader;
			uint32_t ref; /// Reference-count
		};

		D3D11RenderDevice* _render_device;

		map<uint64_t, ProgramEntry<ID3D11VertexShader>> _vertex_shaders;
		map<uint64_t, ProgramEntry<ID3D11HullShader>> _hull_shaders;
		map<uint64_t, ProgramEntry<ID3D11DomainShader>> _domain_shaders;
		map<uint64_t, ProgramEntry<ID3D11GeometryShader>> _geometry_shaders;
		map<uint64_t, ProgramEntry<ID3D11PixelShader>> _pixel_shaders;
		map<uint64_t, ProgramEntry<ID3D11ComputeShader>> _compute_shaders;

	};


} // namespace sb


#endif // __D3D11SHADERPROGRAMMANAGER_H__

