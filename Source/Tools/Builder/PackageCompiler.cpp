// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "PackageCompiler.h"

#include <Foundation/IO/MemoryStream.h>
#include <Foundation/Container/ConfigValue.h>
#include <Foundation/Json/Json.h>

#include <Foundation/Resource/ResourcePackage.h>

namespace sb
{

	PackageCompiler::PackageCompiler(const ConfigValue& config)
		: Compiler(config)
	{
	}
	PackageCompiler::~PackageCompiler()
	{
	}

	CompilerSystem::Result PackageCompiler::Compile(const FilePath& source_file, const FilePath& target_file, const CompilerSystem::CompilerContext& context)
	{
		uint8_t* source_data = 0;
		uint32_t length = 0;

		{
			FileStreamPtr file = context.asset_source->OpenFile(source_file.c_str(), File::READ);
			if (!file.Get() || file->Length() < 0)
				return CompilerSystem::FAILED;

			length = (uint32_t)file->Length();
			source_data = (uint8_t*)memory::Malloc(length); // TODO: Temp alloc
			file->Read(source_data, length);

		}

		ConfigValue package;

		simplified_json::Reader reader;
		if (!reader.Read((const char*)source_data, length, package))
		{
			memory::Free(source_data); // TODO: Temp alloc
			SetError(reader.GetErrorMessage().c_str());

			return CompilerSystem::FAILED;
		}
		memory::Free(source_data); // TODO: Temp alloc

		vector<uint8_t> data;
		DynamicMemoryStream stream(&data);

		package_resource::Compile(package, stream);

		if (!WriteAsset(context.asset_target, target_file, data.data(), (uint32_t)data.size()))
			return CompilerSystem::FAILED;

		return CompilerSystem::SUCCESSFUL;
	}


} // namespace sb

