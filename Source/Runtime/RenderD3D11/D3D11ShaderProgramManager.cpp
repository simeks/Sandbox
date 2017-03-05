// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "D3D11ShaderProgramManager.h"
#include "D3D11RenderDevice.h"

#include <Foundation/Hash/murmur_hash.h>


namespace sb
{

	D3D11ShaderProgramManager::D3D11ShaderProgramManager(D3D11RenderDevice* render_device)
		: _render_device(render_device)
	{

	}
	D3D11ShaderProgramManager::~D3D11ShaderProgramManager()
	{
		// Check for unreleased shaders
		Assert(_vertex_shaders.size() == 0);
		Assert(_hull_shaders.size() == 0);
		Assert(_domain_shaders.size() == 0);
		Assert(_geometry_shaders.size() == 0);
		Assert(_pixel_shaders.size() == 0);
		Assert(_compute_shaders.size() == 0);
	}


	//-------------------------------------------------------------------------------

	ID3D11VertexShader* D3D11ShaderProgramManager::GetVertexShader(const vector<uint8_t>& byte_code)
	{
		uint64_t hash = murmur_hash_64(byte_code.data(), (uint32_t)byte_code.size(), 0);
		map<uint64_t, ProgramEntry<ID3D11VertexShader>>::iterator it = _vertex_shaders.find(hash);
		if (it != _vertex_shaders.end())
		{
			it->second.ref++;
			return it->second.shader;
		}

		ProgramEntry<ID3D11VertexShader> program;
		program.ref = 1;

		D3D_VERIFY(_render_device->GetD3DDevice()->CreateVertexShader(byte_code.data(), (uint32_t)byte_code.size(), NULL, &program.shader));

		_vertex_shaders[hash] = program;
		return program.shader;
	}

	ID3D11HullShader* D3D11ShaderProgramManager::GetHullShader(const vector<uint8_t>& byte_code)
	{
		uint64_t hash = murmur_hash_64(byte_code.data(), (uint32_t)byte_code.size(), 0);
		map<uint64_t, ProgramEntry<ID3D11HullShader>>::iterator it = _hull_shaders.find(hash);
		if (it != _hull_shaders.end())
		{
			it->second.ref++;
			return it->second.shader;
		}

		ProgramEntry<ID3D11HullShader> program;
		program.ref = 1;

		D3D_VERIFY(_render_device->GetD3DDevice()->CreateHullShader(byte_code.data(), (uint32_t)byte_code.size(), NULL, &program.shader));

		_hull_shaders[hash] = program;
		return program.shader;
	}

	ID3D11DomainShader* D3D11ShaderProgramManager::GetDomainShader(const vector<uint8_t>& byte_code)
	{
		uint64_t hash = murmur_hash_64(byte_code.data(), (uint32_t)byte_code.size(), 0);
		map<uint64_t, ProgramEntry<ID3D11DomainShader>>::iterator it = _domain_shaders.find(hash);
		if (it != _domain_shaders.end())
		{
			it->second.ref++;
			return it->second.shader;
		}

		ProgramEntry<ID3D11DomainShader> program;
		program.ref = 1;

		D3D_VERIFY(_render_device->GetD3DDevice()->CreateDomainShader(byte_code.data(), (uint32_t)byte_code.size(), NULL, &program.shader));

		_domain_shaders[hash] = program;
		return program.shader;
	}

	ID3D11GeometryShader* D3D11ShaderProgramManager::GetGeometryShader(const vector<uint8_t>& byte_code)
	{
		uint64_t hash = murmur_hash_64(byte_code.data(), (uint32_t)byte_code.size(), 0);
		map<uint64_t, ProgramEntry<ID3D11GeometryShader>>::iterator it = _geometry_shaders.find(hash);
		if (it != _geometry_shaders.end())
		{
			it->second.ref++;
			return it->second.shader;
		}

		ProgramEntry<ID3D11GeometryShader> program;
		program.ref = 1;

		D3D_VERIFY(_render_device->GetD3DDevice()->CreateGeometryShader(byte_code.data(), (uint32_t)byte_code.size(), NULL, &program.shader));

		_geometry_shaders[hash] = program;
		return program.shader;
	}

	ID3D11PixelShader* D3D11ShaderProgramManager::GetPixelShader(const vector<uint8_t>& byte_code)
	{
		uint64_t hash = murmur_hash_64(byte_code.data(), (uint32_t)byte_code.size(), 0);
		map<uint64_t, ProgramEntry<ID3D11PixelShader>>::iterator it = _pixel_shaders.find(hash);
		if (it != _pixel_shaders.end())
		{
			it->second.ref++;
			return it->second.shader;
		}

		ProgramEntry<ID3D11PixelShader> program;
		program.ref = 1;

		D3D_VERIFY(_render_device->GetD3DDevice()->CreatePixelShader(byte_code.data(), byte_code.size(), NULL, &program.shader));

		_pixel_shaders[hash] = program;
		return program.shader;
	}

	ID3D11ComputeShader* D3D11ShaderProgramManager::GetComputeShader(const vector<uint8_t>& byte_code)
	{
		uint64_t hash = murmur_hash_64(byte_code.data(), (uint32_t)byte_code.size(), 0);
		map<uint64_t, ProgramEntry<ID3D11ComputeShader>>::iterator it = _compute_shaders.find(hash);
		if (it != _compute_shaders.end())
		{
			it->second.ref++;
			return it->second.shader;
		}

		ProgramEntry<ID3D11ComputeShader> program;
		program.ref = 1;

		D3D_VERIFY(_render_device->GetD3DDevice()->CreateComputeShader(byte_code.data(), (uint32_t)byte_code.size(), NULL, &program.shader));

		_compute_shaders[hash] = program;
		return program.shader;
	}

	template<>
	typename ID3D11VertexShader* D3D11ShaderProgramManager::GetShader<ID3D11VertexShader>(const vector<uint8_t>& byte_code)
	{
		return GetVertexShader(byte_code);
	}
	template<>
	typename ID3D11HullShader* D3D11ShaderProgramManager::GetShader<ID3D11HullShader>(const vector<uint8_t>& byte_code)
	{
		return GetHullShader(byte_code);
	}
	template<>
	typename ID3D11DomainShader* D3D11ShaderProgramManager::GetShader<ID3D11DomainShader>(const vector<uint8_t>& byte_code)
	{
		return GetDomainShader(byte_code);
	}
	template<>
	typename ID3D11GeometryShader* D3D11ShaderProgramManager::GetShader<ID3D11GeometryShader>(const vector<uint8_t>& byte_code)
	{
		return GetGeometryShader(byte_code);
	}
	template<>
	typename ID3D11PixelShader* D3D11ShaderProgramManager::GetShader<ID3D11PixelShader>(const vector<uint8_t>& byte_code)
	{
		return GetPixelShader(byte_code);
	}
	template<>
	typename ID3D11ComputeShader* D3D11ShaderProgramManager::GetShader<ID3D11ComputeShader>(const vector<uint8_t>& byte_code)
	{
		return GetComputeShader(byte_code);
	}

	//-------------------------------------------------------------------------------

	void D3D11ShaderProgramManager::ReleaseShader(ID3D11VertexShader* shader)
	{
		map<uint64_t, ProgramEntry<ID3D11VertexShader>>::iterator it, end;
		it = _vertex_shaders.begin(); end = _vertex_shaders.end();
		for (; it != end; ++it)
		{
			if (it->second.shader == shader)
			{
				if (--(it->second.ref) == 0)
				{
					SAFE_RELEASE(it->second.shader);
					_vertex_shaders.erase(it);
				}
				return;
			}
		}
	}

	void D3D11ShaderProgramManager::ReleaseShader(ID3D11HullShader* shader)
	{
		map<uint64_t, ProgramEntry<ID3D11HullShader>>::iterator it, end;
		it = _hull_shaders.begin(); end = _hull_shaders.end();
		for (; it != end; ++it)
		{
			if (it->second.shader == shader)
			{
				if (--(it->second.ref) == 0)
				{
					SAFE_RELEASE(it->second.shader);
					_hull_shaders.erase(it);
				}
				return;
			}
		}
	}

	void D3D11ShaderProgramManager::ReleaseShader(ID3D11DomainShader* shader)
	{
		map<uint64_t, ProgramEntry<ID3D11DomainShader>>::iterator it, end;
		it = _domain_shaders.begin(); end = _domain_shaders.end();
		for (; it != end; ++it)
		{
			if (it->second.shader == shader)
			{
				if (--(it->second.ref) == 0)
				{
					SAFE_RELEASE(it->second.shader);
					_domain_shaders.erase(it);
				}
				return;
			}
		}
	}

	void D3D11ShaderProgramManager::ReleaseShader(ID3D11GeometryShader* shader)
	{
		map<uint64_t, ProgramEntry<ID3D11GeometryShader>>::iterator it, end;
		it = _geometry_shaders.begin(); end = _geometry_shaders.end();
		for (; it != end; ++it)
		{
			if (it->second.shader == shader)
			{
				if (--(it->second.ref) == 0)
				{
					SAFE_RELEASE(it->second.shader);
					_geometry_shaders.erase(it);
				}
				return;
			}
		}
	}

	void D3D11ShaderProgramManager::ReleaseShader(ID3D11PixelShader* shader)
	{
		map<uint64_t, ProgramEntry<ID3D11PixelShader>>::iterator it, end;
		it = _pixel_shaders.begin(); end = _pixel_shaders.end();
		for (; it != end; ++it)
		{
			if (it->second.shader == shader)
			{
				if (--(it->second.ref) == 0)
				{
					SAFE_RELEASE(it->second.shader);
					_pixel_shaders.erase(it);
				}
				return;
			}
		}
	}

	void D3D11ShaderProgramManager::ReleaseShader(ID3D11ComputeShader* shader)
	{
		map<uint64_t, ProgramEntry<ID3D11ComputeShader>>::iterator it, end;
		it = _compute_shaders.begin(); end = _compute_shaders.end();
		for (; it != end; ++it)
		{
			if (it->second.shader == shader)
			{
				if (--(it->second.ref) == 0)
				{
					SAFE_RELEASE(it->second.shader);
					_compute_shaders.erase(it);
				}
				return;
			}
		}
	}

	//-------------------------------------------------------------------------------

} // namespace sb

