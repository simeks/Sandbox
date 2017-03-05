// Copyright 2008-2014 Simon Ekström

#ifndef __BUILDER_MATERIALCOMPILER_H__
#define __BUILDER_MATERIALCOMPILER_H__

#include "CompilerSystem.h"

namespace sb
{

	struct MaterialData;
	class MaterialCompiler : public CompilerSystem::Compiler
	{
	public:
		MaterialCompiler(const ConfigValue& config);
		~MaterialCompiler();

		CompilerSystem::Result Compile(const FilePath& source_file, const FilePath& target_file, const CompilerSystem::CompilerContext& context);


	private:
		void ParseShaderVariables(const ConfigValue& variables, MaterialData& material_data);

	};

} // namespace sb


#endif // __BUILDER_MATERIALCOMPILER_H__

