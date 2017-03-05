// Copyright 2008-2014 Simon Ekström

#ifndef __BUILDER_SCRIPTCOMPILER_H__
#define __BUILDER_SCRIPTCOMPILER_H__

#include "CompilerSystem.h"

namespace sb
{

	class ScriptCompiler : public CompilerSystem::Compiler
	{
	public:
		ScriptCompiler(const ConfigValue& config);
		~ScriptCompiler();

		CompilerSystem::Result Compile(const FilePath& source_file, const FilePath& target_file, const CompilerSystem::CompilerContext& context);

	};

} // namespace sb



#endif // __BUILDER_SCRIPTCOMPILER_H__

