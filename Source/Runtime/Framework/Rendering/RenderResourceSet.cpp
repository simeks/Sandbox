// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "RenderResourceSet.h"

#include <Engine/Rendering/RenderResourceAllocator.h>
#include <Foundation/Container/ConfigValue.h>


namespace sb
{

	RenderResourceSet::RenderResourceSet()
	{
	}
	RenderResourceSet::~RenderResourceSet()
	{
		Clear();
	}

	void RenderResourceSet::Clear()
	{
		for (auto& resource : _resources)
		{
			if (resource.second.owned)
			{
				if (resource.second.resource)
				{
					Assert(IsInvalid(resource.second.resource->GetHandle()));
					delete resource.second.resource;
				}
			}
		}
		_resources.clear();
	}

	void RenderResourceSet::Load(const ConfigValue& resources, RenderResourceAllocator* allocator)
	{
		Assert(resources.IsArray());

		for (uint32_t r = 0; r < resources.Size(); ++r)
		{
			Assert(resources[r].IsObject());
			const ConfigValue& resource = resources[r];

			Assert(resource["name"].IsString());
			StringId32 name = resource["name"].AsString();

			ResourceMap::iterator it = _resources.find(name);
			if (it != _resources.end())
				continue; // Already loaded

			Assert(resource["type"].IsString());
			if (strcmp(resource["type"].AsString(), "render_target") == 0)
			{
				TextureDesc desc;
				desc.pixel_format = image::PF_UNKNOWN;
				desc.type = TextureDesc::TYPE_2D;
				desc.mip_count = 1;
				desc.array_size = 1;

				Assert(resource["format"].IsString());
				desc.pixel_format = image::ParseFormat(resource["format"].AsString());
				Assert(desc.pixel_format != image::PF_UNKNOWN);

				uint32_t bind_flags = RRenderTarget::BIND_RENDER_TARGET;
				switch (desc.pixel_format)
				{
				case image::PF_D16:
				case image::PF_D32F:
				case image::PF_D24S8:
					bind_flags = RRenderTarget::BIND_DEPTH_STENCIL;
					break;
				default:
					break;
				};

				if (resource["array_size"].IsNumber())
				{
					desc.array_size = resource["array_size"].AsUInt();
				}

				if (resource["flags"].IsString() && strcmp(resource["flags"].AsString(), "READWRITE") == 0)
				{
					bind_flags |= RRenderTarget::BIND_UNORDERED_ACCESS;
				}

				RRenderTarget* target = nullptr;
				if (resource["width"].IsNumber() && resource["height"].IsNumber())
				{
					desc.width = resource["width"].AsInt();
					desc.height = resource["height"].AsInt();

					target = new RRenderTarget(desc, bind_flags);
				}
				else if (resource["x_scale"].IsNumber() && resource["y_scale"].IsNumber())
				{
					RRenderTarget* back_buffer = (RRenderTarget*)GetResource("back_buffer");
					Assert(back_buffer);

					float x_scale = resource["x_scale"].AsFloat();
					float y_scale = resource["y_scale"].AsFloat();

					// For scaled render targets we depend on the back buffers dimension.
					//	TODO: Make it possible to specify parent target?
					//	TODO: What if the back buffer changes size?
					TextureDesc back_buffer_desc = back_buffer->GetDesc();
					desc.width = uint32_t(back_buffer_desc.width * x_scale);
					desc.height = uint32_t(back_buffer_desc.height * y_scale);

					target = new RRenderTarget(desc, bind_flags);
				}
				else
				{
					AssertMsg(false, "Unrecognized resource type");
				}

				Resource res;
				res.resource = target;
				res.owned = true;

				_resources[name] = res;

				allocator->AllocateRenderTarget(*target);
			}
			else
			{
				AssertMsg(false, "Unrecognized resource type");
			}
		}
	}

	void RenderResourceSet::Unload(RenderResourceAllocator* allocator)
	{
		for (auto& entry : _resources)
		{
			// Only allocate resources that are both owned and allocated
			if (entry.second.owned && IsValid(entry.second.resource->GetHandle()))
			{
				allocator->ReleaseResource(*entry.second.resource);
			}
		}
	}
	void RenderResourceSet::AddExternal(StringId32 name, RenderResource* resource)
	{
		ResourceMap::iterator it = _resources.find(name);
		if (it != _resources.end())
		{
			// Only replace existing resources if they are external
			if (it->second.owned == false)
				it->second.resource = resource;
		}

		Resource res;
		res.owned = false;
		res.resource = resource;
		_resources[name] = res;
	}


	RenderResource* RenderResourceSet::GetResource(StringId32 name)
	{
		ResourceMap::const_iterator it = _resources.find(name);
		if (it != _resources.end())
			return it->second.resource;

		return nullptr;
	}

} // namespace sb

