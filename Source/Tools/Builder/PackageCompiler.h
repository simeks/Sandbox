// Copyright 2008-2014 Simon Ekström

#ifndef __BUILDER_PACKAGECOMPILER_H__
#define __BUILDER_PACKAGECOMPILER_H__


#include "CompilerSystem.h"

namespace sb
{

	/// Compiler for resource packages
	class PackageCompiler : public CompilerSystem::Compiler
	{
	public:
		PackageCompiler(const ConfigValue& config);
		~PackageCompiler();

		CompilerSystem::Result Compile(const FilePath& source_file, const FilePath& target_file, const CompilerSystem::CompilerContext& context);

	};

} // namespace sb

#endif // __BUILDER_PACKAGECOMPILER_H__

