// Copyright 2008-2014 Simon Ekström

#ifndef __FRAMEWORK_RENDERVIEWSET_H__
#define __FRAMEWORK_RENDERVIEWSET_H__


namespace sb
{

	class RenderView;
	class ConfigValue;
	class RenderResourceAllocator;
	class RenderResourceSet;

	class RenderViewSet
	{
	public:
		RenderViewSet();
		~RenderViewSet();

		void Load(	const ConfigValue& config, 
					RenderResourceAllocator* resource_allocator,
					RenderResourceSet* resource_set);

		void Unload(RenderResourceAllocator* resource_allocator);

		RenderView* Get(StringId32 name);

	private:
		map<StringId32, RenderView*> _views;


	};

} // namespace sb


#endif // __FRAMEWORK_RENDERVIEWSET_H__
