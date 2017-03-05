// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "LightComponent.h"
#include "GameObject.h"
#include "Transform.h"
#include "World.h"
#include "RenderWorld.h"


#include <Foundation/Math/AABB.h>


namespace sb
{

	//-------------------------------------------------------------------------------
	LightComponent::LightComponent()
		: RenderComponent(RenderComponent::LIGHT),
		_light_type(LT_POINT),
		_flags(0),
		_range(10.0f),
		_direction(1.0f, 0.0f, 0.0f),
		_color(1.0f, 1.0f, 1.0f),
		_spot_light_angle(30.0f * ((float)MATH_PI / 180.0f))
	{
		UpdateBounds();
	}
	LightComponent::~LightComponent()
	{
	}
	//-------------------------------------------------------------------------------

	void LightComponent::CalculateBounds(AABB& bounding_box) const
	{
		Vec3f light_position = Vec3f(0.0f, 0.0f, 0.0f);
		if (_game_object)
		{
			light_position = GetTransform().GetLocalPosition();
		}

		switch (_light_type)
		{
		case LightComponent::LT_POINT:
		{
			Vec3f center = light_position;
			bounding_box.min = Vec3f(center.x - _range, center.y - _range, center.z - _range);
			bounding_box.max = Vec3f(center.x + _range, center.y + _range, center.z + _range);
		}
			break;
		case LightComponent::LT_SPOT:
		{
			Vec3f top = light_position;
			bounding_box.min.x = top.x; bounding_box.min.y = top.y; bounding_box.min.z = top.z;
			bounding_box.max.x = top.x; bounding_box.max.y = top.y; bounding_box.max.z = top.z;

			Vec3f bottom = top + _direction * _range;
			float r = _range * math::Tan(_spot_light_angle);

			Vec3f xaxis = _direction.Cross(Vec3f::UNIT_Y);
			if (xaxis.Length() == 0)
				xaxis = _direction.Cross(Vec3f::UNIT_Z);
			Vec3f yaxis = xaxis.Cross(_direction);

			Vec3f c1 = bottom + (xaxis * r);
			Vec3f c2 = bottom + (xaxis * -r);
			Vec3f c3 = bottom + (yaxis * r);
			Vec3f c4 = bottom + (yaxis * -r);

			bounding_box.Merge(c1);
			bounding_box.Merge(c2);
			bounding_box.Merge(c3);
			bounding_box.Merge(c4);

		}
			break;
		default:
			Assert(false);
		}
	}

	//-------------------------------------------------------------------------------
	void LightComponent::SetLightType(LightType type)
	{
		_light_type = type;

		UpdateBounds();
	}
	LightComponent::LightType LightComponent::GetLightType() const
	{
		return _light_type;
	}

	void LightComponent::SetDirection(const Vec3f& direction)
	{
		Assert(_direction.Length() != 0.0f);
		Assert(_light_type == LT_DIRECTIONAL || _light_type == LT_SPOT);
		_direction = direction;
		_direction.Normalize();

		UpdateBounds();
	}
	const Vec3f& LightComponent::GetDirection() const
	{
		Assert(_light_type == LT_DIRECTIONAL || _light_type == LT_SPOT);
		return _direction;
	}

	void LightComponent::SetRange(float range)
	{
		Assert(_light_type == LT_POINT || _light_type == LT_SPOT);
		_range = range;

		UpdateBounds();
	}
	float LightComponent::GetRange() const
	{
		return _range;
	}

	void LightComponent::SetColor(Vec3f color)
	{
		_color = color;
	}
	const Vec3f& LightComponent::GetColor() const
	{
		return _color;
	}

	void LightComponent::SetSpotlightAngle(float angle)
	{
		Assert(_light_type == LT_SPOT);
		_spot_light_angle = angle;

		UpdateBounds();
	}
	float LightComponent::GetSpotlightAngle() const
	{
		Assert(_light_type == LT_SPOT);
		return _spot_light_angle;
	}

	void LightComponent::UpdateBounds()
	{
		if (_light_type == LT_DIRECTIONAL)
		{
			_game_object->GetWorld()->GetRenderWorld()->SetVisibilityFlags(_render_handle, culling::ALWAYS_VISIBLE);
		}
		else
		{
			AABB aabb;
			CalculateBounds(aabb);
			_render_world->SetAABB(_render_handle, aabb);

			_game_object->GetWorld()->GetRenderWorld()->SetVisibilityFlags(_render_handle, 0);
		}
	}

	void LightComponent::GetBounds(AABB& aabb) const
	{
		CalculateBounds(aabb);
	}
	uint8_t LightComponent::GetVisibilityFlags() const
	{
		return 0;
	}

	//-------------------------------------------------------------------------------

} // namespace sb
