// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "CopyCompiler.h"

namespace sb
{

	CopyCompiler::CopyCompiler(const ConfigValue& config)
		: Compiler(config)
	{
	}
	CopyCompiler::~CopyCompiler()
	{
	}

	CompilerSystem::Result CopyCompiler::Compile(const FilePath& source_file, const FilePath& target_file, const CompilerSystem::CompilerContext& context)
	{
		uint8_t* data = 0;
		uint32_t length = 0;

		{
			FileStreamPtr file = context.asset_source->OpenFile(source_file.c_str(), File::READ);
			if (!file.Get() || file->Length() < 0)
				return CompilerSystem::FAILED;

			length = (uint32_t)file->Length();
			data = (uint8_t*)memory::Malloc(length); // TODO: Temp Alloc
			file->Read(data, length);
		}

		if (!WriteAsset(context.asset_target, target_file, data, length))
		{
			memory::Free(data); // TODO: Temp Alloc
			return CompilerSystem::FAILED;
		}
		memory::Free(data); // TODO: Temp Alloc

		return CompilerSystem::SUCCESSFUL;
	}

} // namespace sb


