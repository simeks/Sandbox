// Copyright 2008-2014 Simon Ekström

#ifndef __BUILDER_MESHCOMPILER_H__
#define __BUILDER_MESHCOMPILER_H__

#include "CompilerSystem.h"

namespace sb
{

	class MeshCompiler : public CompilerSystem::Compiler
	{
	public:
		MeshCompiler(const ConfigValue& config);
		~MeshCompiler();

		CompilerSystem::Result Compile(const FilePath& source_file, const FilePath& target_file, const CompilerSystem::CompilerContext& context);


	private:


	};

} // namespace sb


#endif // __BUILDER_MESHCOMPILER_H__
