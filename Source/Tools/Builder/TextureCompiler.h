// Copyright 2008-2014 Simon Ekström

#ifndef __BUILDER_TEXTURECOMPILER_H__
#define __BUILDER_TEXTURECOMPILER_H__

#include "CompilerSystem.h"

#include <nvtt/nvtt.h>

namespace sb
{

	class TextureCompiler : public CompilerSystem::Compiler
	{
	public:
		TextureCompiler(const ConfigValue& config);
		~TextureCompiler();

		CompilerSystem::Result Compile(const FilePath& source_file, const FilePath& target_file, const CompilerSystem::CompilerContext& context);


	private:

		nvtt::Format TranslateFormat(const char* str);

	};

} // namespace sb


#endif // __BUILDER_TEXTURECOMPILER_H__

