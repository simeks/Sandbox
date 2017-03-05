// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERRESOURCESET_H__
#define __RENDERRESOURCESET_H__


namespace sb
{

	class ConfigValue;
	class RenderResource;
	class RenderResourceAllocator;

	/// @brief A set of render resources mapped by a StringId32
	class RenderResourceSet
	{
	public:
		RenderResourceSet();
		~RenderResourceSet();

		void Load(const ConfigValue& resources, RenderResourceAllocator* allocator);
		void Unload(RenderResourceAllocator* allocator);


		/// Adds an external resource to the set, will replace any already existing entries
		///		given that they also are external, will do nothing if the resource is owned
		///		by this set.
		void AddExternal(StringId32 name, RenderResource* resource);

		/// Clears the set, Make sure to use ReleaseResources before clearing
		/// @remark Will assert if any owned resources are still allocated
		void Clear();

		/// @brief Returns the resource with the given identifier,
		///	@return NULL if the resource can't be found
		RenderResource* GetResource(StringId32 name);

	private:
		const RenderResourceSet& operator=(const RenderResourceSet&) { return *this; }

		struct Resource
		{
			Resource() : resource(nullptr), owned(false) {}

			RenderResource* resource;
			bool owned; // Is resource owned by this set or is it external (Like the back buffer)
		};

		typedef map<StringId32, Resource> ResourceMap;

		ResourceMap _resources;
	};

} // namespace sb


#endif // __RENDERRESOURCESET_H__
