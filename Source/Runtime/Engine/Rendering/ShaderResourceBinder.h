// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_SHADERRESOURCEBINDER_H__
#define __RENDERING_SHADERRESOURCEBINDER_H__

#include "ShaderVariable.h"

#include <Foundation/Math/Matrix4x4.h>

namespace sb
{

	struct ShaderData;
	struct ShaderResources;

	class ShaderParameters;

	/// Helper class for binding shader resources
	class ShaderResourceBinder
	{
	public:
		ShaderResourceBinder(ShaderData* shader_data);
		~ShaderResourceBinder();

		/// Updates all per-object variables.
		void BindAutoVariables(ShaderResources* resources, const Mat4x4f& world_matrix) const;

		/// Binds parameters from the specified parameter container.
		void Bind(ShaderResources* resources, const ShaderParameters& parameters) const;

	private:
		typedef map<StringId32, ShaderVariable> VariableMap;

		/// Types for auto-variables like the world matrix
		enum ConstantType
		{
			WORLD, // World matrix
			UNKNOWN
		};

		struct AutoConstantBinder
		{
			ConstantType type;
			uint32_t offset; // Offset in the constant data for this variable, in bytes.
		};

		struct ResourceBinder
		{
			StringId32 name;
			uint32_t index;
		};

		void ParseVariables(const vector<ShaderVariable>& variables,
							uint32_t constant_offset,
							vector<AutoConstantBinder>& binders,
							VariableMap& out_variables);

		vector<AutoConstantBinder> _auto_constant_binders;
		vector<AutoConstantBinder> _instance_auto_constant_binders;

		VariableMap _variables;
		VariableMap _instance_variables; // Instance data variables

		vector<ResourceBinder> _resource_binders;

		/// Returns the ConstantType for the specified variable, returns UNKNOWN if no type was found.
		ConstantType GetAutoConstantType(const ShaderVariable& variable);

	};


} // namespace sb


#endif // __RENDERING_SHADERRESOURCEBINDER_H__
