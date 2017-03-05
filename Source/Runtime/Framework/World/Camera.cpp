// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"

#include <Foundation/Math/Math.h>
#include <Foundation/Math/MatrixUtil.h>



namespace sb
{

	//-------------------------------------------------------------------------------
	Camera::Camera()
		: _near_range(CAMERA_DEFAULT_NEAR),
		_far_range(CAMERA_DEFAULT_FAR),
		_vertical_fov(CAMERA_DEFAULT_FOV),
		_projection_type(PERSPECTIVE),
		_width(1.0f),
		_height(1.0f)
	{
		_world_matrix.Identity();
		_view_matrix.Identity();
		_projection_matrix.Identity();

	}
	Camera::~Camera()
	{
	}

	//-------------------------------------------------------------------------------
	void Camera::RegisterComponent()
	{
		_game_object->GetTransform().AttachChild(&_transform);
	}
	void Camera::UnregisterComponent()
	{
		_transform.Detach();
	}
	//-------------------------------------------------------------------------------
	void Camera::Update()
	{
		if (_game_object)
		{
			_world_matrix = _transform.GetWorld();
			_view_matrix = _world_matrix.GetInverted();
		}

		Calculate();
	}
	//-------------------------------------------------------------------------------

	void Camera::SetPerspective(float zn, float zf, float aspect, float fov)
	{
		_projection_type = PERSPECTIVE;
		_near_range = zn;
		_far_range = zf;
		_aspect = aspect;
		_vertical_fov = fov;

		_projection_matrix = matrix_util::CreatePerspectiveFov(_vertical_fov, _aspect, _near_range, _far_range);
	}
	void Camera::SetOrtographic(float zn, float zf, float width, float height)
	{
		_projection_type = ORTHOGRAPHIC;
		_near_range = zn;
		_far_range = zf;
		_width = width;
		_height = height;

		_projection_matrix = matrix_util::CreateOrtho(_width, _height, _near_range, _far_range);
	}

	//-------------------------------------------------------------------------------
	void Camera::SetViewMatrix(const Mat4x4f& view_matrix)
	{
		if (_game_object)
		{
			Mat4x4f world = view_matrix.GetInverted();

			_transform.SetLocalPosition(world.GetTranslation());
			_transform.SetLocalRotation(Mat3x3f(world.m00, world.m01, world.m02,
				world.m10, world.m11, world.m12,
				world.m20, world.m21, world.m22));
			_transform.SetLocalScale(Vec3f(1.0f, 1.0f, 1.0f));
		}
		else
		{
			_view_matrix = view_matrix;
			_world_matrix = _view_matrix.GetInverted();
		}
	}
	void Camera::SetProjectionMatrix(const Mat4x4f& projection_matrix)
	{
		_projection_matrix = projection_matrix;
	}
	//-------------------------------------------------------------------------------
	float Camera::GetNearRange() const
	{
		return _near_range;
	}
	float Camera::GetFarRange() const
	{
		return _far_range;
	}
	float Camera::GetVerticalFov() const
	{
		return _vertical_fov;
	}
	Camera::Projection Camera::GetProjectionType() const
	{
		return _projection_type;
	}

	//-------------------------------------------------------------------------------
	const Mat4x4f& Camera::GetViewMatrix() const
	{
		return _view_matrix;
	}
	const Mat4x4f& Camera::GetProjMatrix() const
	{
		return _projection_matrix;
	}
	const Mat4x4f& Camera::GetWorldMatrix() const
	{
		return _world_matrix;
	}
	const Frustum& Camera::GetFrustum() const
	{
		return _frustum;
	}

	const Transform& Camera::GetTransform() const
	{
		return _transform;
	}
	Transform& Camera::GetTransform()
	{
		return _transform;
	}
	//-------------------------------------------------------------------------------
	void Camera::CalculateFrustumCorners(Vec3f* corners) const
	{
		float near_y, near_x;
		float far_y, far_x;
		if (_projection_type == PERSPECTIVE)
		{
			float tan_half_fovy = math::Tan(_vertical_fov * 0.5f);

			near_y = _near_range * tan_half_fovy;
			near_x = _aspect * near_y;

			far_y = _far_range * tan_half_fovy;
			far_x = _aspect * far_y;
		}
		else // if(m_projType == ORTOGRAPHIC)
		{
			near_x = _width * 0.5f;
			near_y = _height * 0.5f;
			far_x = _width * 0.5f;
			far_y = _height * 0.5f;
		}

		// Near plane
		corners[0].x = -near_x;	corners[0].y = -near_y;	corners[0].z = -_near_range;
		corners[1].x = near_x;	corners[1].y = -near_y;	corners[1].z = -_near_range;
		corners[2].x = -near_x;	corners[2].y = near_y;	corners[2].z = -_near_range;
		corners[3].x = near_x;	corners[3].y = near_y;	corners[3].z = -_near_range;

		// Far plane
		corners[4].x = -far_x;	corners[4].y = -far_y;	corners[4].z = -_far_range;
		corners[5].x = far_x;	corners[5].y = -far_y;	corners[5].z = -_far_range;
		corners[6].x = -far_x;	corners[6].y = far_y;	corners[6].z = -_far_range;
		corners[7].x = far_x;	corners[7].y = far_y;	corners[7].z = -_far_range;


	}
	//-------------------------------------------------------------------------------
	void Camera::Calculate()
	{
		// Calculate frustum in world space
		Mat4x4f view_proj = _projection_matrix * _view_matrix;
		frustum::CalculateFrustum(view_proj, _frustum);

	}

	//-------------------------------------------------------------------------------

} // namespace sb

