// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "ScriptCompiler.h"

namespace sb
{

	ScriptCompiler::ScriptCompiler(const ConfigValue& config)
		: CompilerSystem::Compiler(config)
	{
	}
	ScriptCompiler::~ScriptCompiler()
	{
	}

	CompilerSystem::Result ScriptCompiler::Compile(const FilePath&, const FilePath&,
		const CompilerSystem::CompilerContext&)
	{
		SetError("Not implemented, use copy compiler for scripts");

		return CompilerSystem::FAILED;
	}

} // namespace sb

