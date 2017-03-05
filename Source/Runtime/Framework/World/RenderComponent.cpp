// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "RenderComponent.h"
#include "GameObject.h"
#include "World.h"
#include "RenderWorld.h"


namespace sb
{

	RenderComponent::RenderComponent(Type type)
		: _render_world(nullptr),
		_render_type(type),
		_render_handle(Invalid<uint32_t>())
	{
	}
	RenderComponent::~RenderComponent()
	{
	}

	void RenderComponent::RegisterComponent()
	{
		AABB aabb;
		GetBounds(aabb);

		uint8_t flags = GetVisibilityFlags();

		_render_world = _game_object->GetWorld()->GetRenderWorld();
		_render_handle = _render_world->AddObject(this, aabb, flags);

		_game_object->GetTransform().AttachChild(&_transform);
	}
	void RenderComponent::UnregisterComponent()
	{
		_transform.Detach();

		Assert(_render_world);
		_render_world->RemoveObject(_render_handle);
		_render_handle = Invalid<uint32_t>();
	}

	RenderComponent::Type RenderComponent::GetRenderType() const
	{
		return _render_type;
	}

	const Transform& RenderComponent::GetTransform() const
	{
		return _transform;
	}
	Transform& RenderComponent::GetTransform()
	{
		return _transform;
	}

} // namespace sb

