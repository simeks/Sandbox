// Copyright 2008-2014 Simon Ekström

#ifndef __BUILDER_COPYASSET_H__
#define __BUILDER_COPYASSET_H__

#include "CompilerSystem.h"

namespace sb
{

	/// Basic compiler that just reads the source file into the memory
	///		and returns the data untouched.
	class CopyCompiler : public CompilerSystem::Compiler
	{
	public:
		CopyCompiler(const ConfigValue& config);
		~CopyCompiler();

		CompilerSystem::Result Compile(const FilePath& source_file, const FilePath& target_file, const CompilerSystem::CompilerContext& context);

	};

} // namespace sb



#endif // __BUILDER_COPYASSET_H__

