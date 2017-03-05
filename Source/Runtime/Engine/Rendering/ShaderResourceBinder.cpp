// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "ShaderResourceBinder.h"
#include "Shader.h"
#include "ShaderParameters.h"


#include <Engine/Rendering/RShader.h>
#include <Engine/Rendering/RTexture.h>

namespace sb
{

	ShaderResourceBinder::ShaderResourceBinder(ShaderData* shader_data)
	{
		// As all cbuffers are stored in the same memory space we keep track of the offset from the start for each constant.
		uint32_t constant_offset = 0;
		for (uint32_t cb = 0; cb < shader_data->constant_buffer_reflections.size(); ++cb)
		{
			const ConstantBufferReflection& reflection = shader_data->constant_buffer_reflections[cb];

			if (reflection.type == RConstantBuffer::TYPE_GLOBAL)
				continue;

			ParseVariables(reflection.variables, constant_offset, _auto_constant_binders, _variables);

			constant_offset += shader_data->constant_buffer_reflections[cb].size;
		}

		// Instance data
		if (shader_data->instance_data_reflection.size)
		{
			ParseVariables(shader_data->instance_data_reflection.variables, 
						   0, 
						   _instance_auto_constant_binders,
						   _instance_variables);
		}

		for (uint32_t r = 0; r < shader_data->resource_reflection.resources.size(); ++r)
		{
			ResourceBinder binder;
			binder.name = shader_data->resource_reflection.resources[r].name;
			binder.index = shader_data->resource_reflection.resources[r].index;

			_resource_binders.push_back(binder);
		}

	}

	ShaderResourceBinder::~ShaderResourceBinder()
	{
	}

	void ShaderResourceBinder::BindAutoVariables(ShaderResources* resources, const Mat4x4f& world_matrix) const
	{
		for (auto& binder : _auto_constant_binders)
		{
			void* dst = resources->constant_buffer_data;
			switch (binder.type)
			{
			case WORLD:
				memcpy(memory::PointerAdd(dst, binder.offset), &world_matrix, sizeof(Mat4x4f));
				break;
			case UNKNOWN:
				break;
			};
		}

		for (auto& binder : _instance_auto_constant_binders)
		{
			void* dst = resources->instance_data;
			switch (binder.type)
			{
			case WORLD:
				memcpy(memory::PointerAdd(dst, binder.offset), &world_matrix, sizeof(Mat4x4f));
				break;
			case UNKNOWN:
				break;
			};
		}
	}

	void ShaderResourceBinder::Bind(ShaderResources* resources, const ShaderParameters& parameters) const
	{
		void* constant_data_dest = resources->constant_buffer_data;
		for (auto& var : _variables)
		{
			ShaderParameters::VariableMap::const_iterator source_it = parameters.GetVariableMap().find(var.second.name);
			if (source_it != parameters.GetVariableMap().end())
			{
				memcpy(memory::PointerAdd(constant_data_dest, var.second.offset),
					memory::PointerAdd(parameters.GetVariableData().data(), source_it->second.offset),
					shader_variable::GetSize(var.second.var_class) * var.second.elements);
			}

		}

		constant_data_dest = resources->instance_data;
		for (auto& var : _instance_variables)
		{
			ShaderParameters::VariableMap::const_iterator source_it = parameters.GetVariableMap().find(var.second.name);
			if (source_it != parameters.GetVariableMap().end())
			{
				memcpy(memory::PointerAdd(constant_data_dest, var.second.offset),
					memory::PointerAdd(parameters.GetVariableData().data(), source_it->second.offset),
					shader_variable::GetSize(var.second.var_class) * var.second.elements);
			}

		}

		for (auto& binder : _resource_binders)
		{
			Assert(binder.index < resources->num_resources);

			ShaderParameters::ResourceMap::const_iterator source_it = parameters.GetResourceMap().find(binder.name);
			if (source_it != parameters.GetResourceMap().end())
			{
				resources->resources[binder.index] = source_it->second;
			}
		}
	}

	ShaderResourceBinder::ConstantType ShaderResourceBinder::GetAutoConstantType(const ShaderVariable& variable)
	{
		ConstantType type = UNKNOWN;

		if (variable.name == "world")
		{
			Assert(variable.var_class == ShaderVariable::MATRIX4X4);
			type = WORLD;
		}

		return type;
	}

	void ShaderResourceBinder::ParseVariables(const vector<ShaderVariable>& variables,
											  uint32_t constant_offset,
											  vector<AutoConstantBinder>& binders,
											  VariableMap& out_variables)
	{
		for (auto& var : variables)
		{
			AutoConstantBinder binder;
			binder.offset = constant_offset + var.offset;
			binder.type = GetAutoConstantType(var);

			if (binder.type != UNKNOWN)
				binders.push_back(binder);

			ShaderVariable new_var;
			new_var = var;
			new_var.offset = constant_offset + var.offset;
			out_variables.insert(pair<StringId32, ShaderVariable>(new_var.name, new_var));
		}
	}


} // namespace sb

