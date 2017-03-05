// Copyright 2008-2014 Simon Ekström

#ifndef __BUILDER_FONTCOMPILER_H__
#define __BUILDER_FONTCOMPILER_H__

#include "CompilerSystem.h"

namespace sb
{

class FontCompiler : public CompilerSystem::Compiler
{
public:
	FontCompiler(const ConfigValue& config);
	~FontCompiler();

	CompilerSystem::Result Compile(const FilePath& source_file, const FilePath& target_file, const CompilerSystem::CompilerContext& context);


};


} // namespace sb

#endif // __BUILDER_FONTCOMPILER_H__
