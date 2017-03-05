// Copyright 2008-2014 Simon Ekström

#ifndef __FRAMEWORK_CAMERA_H__
#define __FRAMEWORK_CAMERA_H__

#include <Foundation/Math/Matrix4x4.h>
#include <Foundation/Math/Plane.h>
#include <Foundation/Math/Frustum.h>

#include "Component.h"
#include "Transform.h"

#define CAMERA_DEFAULT_FOV (30.0f * (float)MATH_PI/180.0f)
#define CAMERA_DEFAULT_NEAR 0.25f
#define CAMERA_DEFAULT_FAR 1024.0f


namespace sb
{

	class Camera : public Component
	{
	public:
		enum Projection { PERSPECTIVE, ORTHOGRAPHIC };

		Camera();
		~Camera();

		void RegisterComponent() OVERRIDE;
		void UnregisterComponent() OVERRIDE;

		void Update();

		void SetPerspective(float zn, float zf, float aspect, float fov);
		void SetOrtographic(float zn, float zf, float width, float height);

		void SetViewMatrix(const Mat4x4f& view_matrix);
		void SetProjectionMatrix(const Mat4x4f& projection_matrix);

		float GetNearRange() const;
		float GetFarRange() const;
		float GetVerticalFov() const;
		Projection GetProjectionType() const;

		const Mat4x4f& GetViewMatrix() const;
		const Mat4x4f& GetProjMatrix() const;
		const Mat4x4f& GetWorldMatrix() const;
		const Frustum& GetFrustum() const;

		const Transform& GetTransform() const;
		Transform& GetTransform();

		/// @brief Finds and returns the corners of this cameras frustum in the cameras view space.
		/// @param corners Array big enough to fit every corner of the frustum (8 corner)
		void CalculateFrustumCorners(Vec3f* corners) const;

	private:
		// Calculates all necessary matrices and the frustum
		void Calculate();

		Transform _transform;

		Mat4x4f _view_matrix;
		Mat4x4f _projection_matrix;
		Mat4x4f _world_matrix;
		Frustum _frustum; // Frustum in world space

		float _near_range;
		float _far_range;
		float _vertical_fov; // Vertical fov
		float _aspect;

		// For orthogonal projection
		float _width;
		float _height;

		Projection _projection_type;


	};

} // namespace sb


#endif // __FRAMEWORK_CAMERA_H__
