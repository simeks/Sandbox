// Copyright 2008-2014 Simon Ekström

#ifndef __FRAMEWORK_RENDERCOMPONENT_H__
#define __FRAMEWORK_RENDERCOMPONENT_H__

#include "Component.h"
#include "Transform.h"


namespace sb
{

	class AABB;
	class World;
	class RenderWorld;
	class RenderComponent : public Component
	{
	public:
		enum Type
		{
			LIGHT = 0,
			MESH = 1
		};

		RenderComponent(Type type);
		virtual ~RenderComponent();

		virtual void GetBounds(AABB& aabb) const = 0;
		virtual uint8_t GetVisibilityFlags() const = 0;

		virtual void RegisterComponent() OVERRIDE;
		virtual void UnregisterComponent() OVERRIDE;

		Type GetRenderType() const;

		const Transform& GetTransform() const;
		Transform& GetTransform();

	protected:
		RenderWorld* _render_world;

		Type _render_type;
		uint32_t _render_handle;

		Transform _transform;
	};

} // namespace sb


#endif // __FRAMEWORK_RENDERCOMPONENT_H__
