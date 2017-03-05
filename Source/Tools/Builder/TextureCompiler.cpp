// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "TextureCompiler.h"
#include "DependencyDatabase.h"

#include <Foundation/Container/ConfigValue.h>
#include <Foundation/IO/MemoryStream.h>
#include <Foundation/Json/Json.h>
#include <Foundation/IO/InputBuffer.h>

#include <Engine/Rendering/Texture.h>
#include <Engine/Rendering/DDSImage.h>

#include <nvimage/DirectDrawSurface.h>
#include <nvimage/Image.h>


namespace sb
{

	namespace
	{
		/// Error handler.
		struct ErrorHandler : public nvtt::ErrorHandler
		{
			void error(nvtt::Error e)
			{
				logging::Warning("nvtt: %s", nvtt::errorString(e));
			}
		};


	};


	nvtt::Format TextureCompiler::TranslateFormat(const char* str)
	{
		if (strcmp(str, "RGB") == 0)
		{
			return nvtt::Format_RGB;
		}
		if (strcmp(str, "BC1") == 0)
		{
			return nvtt::Format_BC1;
		}
		if (strcmp(str, "BC2") == 0)
		{
			return nvtt::Format_BC2;
		}
		if (strcmp(str, "BC3") == 0)
		{
			return nvtt::Format_BC3;
		}
		if (strcmp(str, "BC4") == 0)
		{
			return nvtt::Format_BC4;
		}
		if (strcmp(str, "BC5") == 0)
		{
			return nvtt::Format_BC5;
		}

		logging::Warning("Texture format '%s' not recognized, defaulting to RGB", str);
		return nvtt::Format_RGB;
	}


	TextureCompiler::TextureCompiler(const ConfigValue& config)
		: CompilerSystem::Compiler(config)
	{
	}
	TextureCompiler::~TextureCompiler()
	{
	}
	CompilerSystem::Result TextureCompiler::Compile(const FilePath& source_file, const FilePath& target_file, const CompilerSystem::CompilerContext& context)
	{
		ConfigValue texture_node;

		simplified_json::Reader reader;
		if (!reader.ReadFile(context.asset_source, source_file.c_str(), texture_node))
		{
			SetError(reader.GetErrorMessage().c_str());
			return CompilerSystem::FAILED;
		}

		TextureDesc::Type tex_type = TextureDesc::TYPE_2D;
		if (texture_node["type"].IsString() && strcmp(texture_node["type"].AsString(), "cubemap") == 0)
		{
			tex_type = TextureDesc::TYPE_CUBE;
		}

		vector<FilePath> texture_paths;
		if (tex_type == TextureDesc::TYPE_CUBE && (texture_node["file"].IsArray() && texture_node["file"].Size() == 6))
		{
			for (uint32_t i = 0; i < texture_node["file"].Size(); ++i)
			{
				if (!texture_node["file"][i].IsString())
				{
					SetError("Invalid file input, expecting string");
					return CompilerSystem::FAILED;
				}

				FilePath texture_path;
				texture_path = source_file.Directory();
				texture_path += texture_node["file"][i].AsString();
				texture_path.SetSeparator('/');
				context.dependency_database->AddDependent(texture_path.c_str(), source_file.c_str());

				texture_path = context.asset_source->GetFullPath(); // Get the full path
				texture_path += "/";
				texture_path += source_file.Directory();
				texture_path += texture_node["file"][i].AsString();
				texture_path.SetSeparator('/');

				texture_paths.push_back(texture_path);
			}
		}
		else if (tex_type == TextureDesc::TYPE_2D && texture_node["file"].IsString())
		{
			FilePath texture_path;
			texture_path = source_file.Directory();
			texture_path += texture_node["file"].AsString();
			texture_path.SetSeparator('/');
			context.dependency_database->AddDependent(texture_path.c_str(), source_file.c_str());

			texture_path = context.asset_source->GetFullPath(); // Get the full path
			texture_path += "/";
			texture_path += source_file.Directory();
			texture_path += texture_node["file"].AsString();
			texture_path.SetSeparator('/');

			texture_paths.push_back(texture_path);
		}
		else
		{
			SetError("No source texture file defined");
			return CompilerSystem::FAILED;
		}

		ErrorHandler error_handler;

		nvtt::InputOptions input_options;
		nvtt::OutputOptions output_options;
		nvtt::CompressionOptions compress_options;

		output_options.setErrorHandler(&error_handler);

		if (texture_node["mip_maps"].IsNumber())
		{
			int mip_maps = texture_node["mip_maps"].AsInt();
			input_options.setMipmapGeneration((mip_maps != 0), mip_maps);
		}


		FilePath target_path;
		target_path = context.asset_target->GetFullPath();
		target_path += "/";
		target_path += target_file.c_str();
		target_path.SetSeparator('/');

		// Make sure directory exists
		context.asset_target->MakeDirectory(target_file.Directory().c_str());

		output_options.setFileName(target_path.c_str());

		// In case we have multiple file sources: Make sure all have the same extension
		string file_ext = texture_paths[0].Extension();
		for (uint32_t i = 1; i < texture_paths.size(); ++i)
		{
			if (texture_paths[i].Extension() != file_ext)
			{
				SetError("Mismatching file extensions, expecting all input files to have the same file type");
				return CompilerSystem::FAILED;
			}
		}

#ifdef SANDBOX_PLATFORM_WIN64
		if (file_ext == "png")
		{
			logging::Error("TextureCompiler ('%s'): PNG not supported in 64-bit (nvimage).", texture_paths[0].c_str());
		}
#endif

		if (file_ext == "dds")
		{
			if (texture_paths.size() != 1)
			{
				SetError("Support for multiple input file for DDS not implemented"); // TODO:
				return CompilerSystem::FAILED;
			}


			nv::DirectDrawSurface dds(texture_paths[0].c_str());
			if (!dds.isValid())
			{
				SetError("Texture file is not a valid dds file");
				return CompilerSystem::FAILED;
			}

			if (!dds.isSupported() || dds.isTexture3D())
			{
				SetError("Texture file is not supported");
				return CompilerSystem::FAILED;
			}

			uint32_t face_count;
			if (dds.isTexture2D())
			{
				input_options.setTextureLayout(nvtt::TextureType_2D, dds.width(), dds.height());
				face_count = 1;
			}
			else if (dds.isTextureCube())
			{
				input_options.setTextureLayout(nvtt::TextureType_Cube, dds.width(), dds.height());
				face_count = 6;
			}
			else
			{
				SetError("Unsupported dds texture type");
				return CompilerSystem::FAILED;
			}

			uint32_t mip_count = dds.mipmapCount();
			nv::Image mipmap;

			for (uint32_t f = 0; f < face_count; ++f)
			{
				for (uint32_t m = 0; m < mip_count; ++m)
				{
					dds.mipmap(&mipmap, f, m);
					input_options.setMipmapData(mipmap.pixels(), mipmap.width(), mipmap.height(), 1, f, m);
				}
			}

		}
		else
		{
			if (tex_type == TextureDesc::TYPE_2D)
			{
				nv::Image image;
				if (!image.load(texture_paths[0].c_str()))
				{
					SetError("Failed to load texture file");
					return CompilerSystem::FAILED;
				}

				input_options.setTextureLayout(nvtt::TextureType_2D, image.width(), image.height());
				input_options.setMipmapData(image.pixels(), image.width(), image.height());
			}
			else if (tex_type == TextureDesc::TYPE_CUBE)
			{
				uint32_t width = 0, height = 0;

				nv::Image image;
				for (uint32_t f = 0; f < 6; ++f)
				{
					if (!image.load(texture_paths[f].c_str()))
					{
						SetError("Failed to load texture file");
						return CompilerSystem::FAILED;
					}

					if (f == 0)
					{
						width = image.width();
						height = image.height();

						input_options.setTextureLayout(nvtt::TextureType_Cube, width, height);
					}
					else if (width != image.width() || height != image.height())
					{
						SetError("All sides of a cube map needs to be the same size");
						return CompilerSystem::FAILED;
					}

					input_options.setMipmapData(image.pixels(), width, height, 1, f);
				}

			}
		}

		if (texture_node["flags"].IsString() && (strcmp(texture_node["flags"].AsString(), "normal") == 0))
			input_options.setNormalMap(true);

		if (texture_node["format"].IsString())
			compress_options.setFormat(TranslateFormat(texture_node["format"].AsString()));
		else
			compress_options.setFormat(nvtt::Format_RGB);

		nvtt::Compressor compressor;
		if (!compressor.process(input_options, compress_options, output_options))
			return CompilerSystem::FAILED;

		return CompilerSystem::SUCCESSFUL;
	}

} // namespace sb

