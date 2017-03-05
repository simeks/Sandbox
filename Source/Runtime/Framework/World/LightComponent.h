// Copyright 2008-2014 Simon Ekström

#ifndef __FRAMEWORK_LIGHTCOMPONENT_H__
#define __FRAMEWORK_LIGHTCOMPONENT_H__

#include "RenderComponent.h"

#include <Foundation/Math/Vec3.h>


namespace sb
{

	class AABB;

	class LightComponent : public RenderComponent
	{
	public:
		enum LightType
		{
			LT_UNKNOWN = 0,
			LT_POINT = 1,
			LT_SPOT = 2,
			LT_DIRECTIONAL = 4

		};
		enum Flags
		{
			CAST_SHADOWS = 0x1
		};


		LightComponent();
		~LightComponent();

		/// Calculates the local bounding box for this lights and puts the result into the parameter bounding_box.
		/// @param bounding_box A variable for holding the result.
		void CalculateBounds(AABB& bounding_box) const;

		void SetLightType(LightType type);
		LightType GetLightType() const;

		void SetDirection(const Vec3f& direction);
		const Vec3f& GetDirection() const;

		void SetRange(float range);
		float GetRange() const;

		void SetColor(Vec3f color);
		const Vec3f& GetColor() const;

		/// Sets the outer angle in radians of the spotlight.
		void SetSpotlightAngle(float angle);

		/// Returns the outer angle in radians of the spotlight.
		float GetSpotlightAngle() const;

		virtual void GetBounds(AABB& aabb) const OVERRIDE;
		virtual uint8_t GetVisibilityFlags() const OVERRIDE;


	private:
		void UpdateBounds();

		LightType _light_type;
		uint32_t _flags;

		float _range;
		Vec3f _direction;
		Vec3f _color;
		float _spot_light_angle;

	};

} // namespace sb


#endif // __FRAMEWORK_LIGHTCOMPONENT_H__
