// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Transform.h"
#include "World.h"


namespace sb
{

	Transform::Transform()
		: _parent(nullptr),
		_rotation(Mat3x3f::CreateIdentity()),
		_position(0.0f, 0.0f, 0.0f),
		_scale(1.0f, 1.0f, 1.0f)
	{

	}
	Transform::~Transform()
	{
		if (_parent)
		{
			_parent->DetachChild(this);
		}

		for (auto& child : _children)
		{
			child->_parent = nullptr;
		}
		_children.clear();
	}

	void Transform::SetLocalRotation(const Mat3x3f& rotation)
	{
		_rotation = rotation;
	}
	void Transform::SetLocalPosition(const Vec3f& position)
	{
		_position = position;
	}
	void Transform::SetLocalScale(const Vec3f& scale)
	{
		_scale = scale;
	}

	const Mat3x3f& Transform::GetLocalRotation() const
	{
		return _rotation;
	}
	const Vec3f& Transform::GetLocalPosition() const
	{
		return _position;
	}
	const Vec3f& Transform::GetLocalScale() const
	{
		return _scale;
	}

	const Mat4x4f& Transform::GetWorld() const
	{
		return _world;
	}
	Transform* Transform::GetParent()
	{
		return _parent;
	}
	void Transform::Detach()
	{
		if (_parent)
		{
			_parent->DetachChild(this);
			_parent = nullptr;
		}
	}

	void Transform::AttachChild(Transform* child)
	{
		child->Detach();

		_children.push_back(child);
		child->_parent = this;
	}
	void Transform::DetachChild(Transform* child)
	{
		vector<Transform*>::iterator it = std::find(_children.begin(), _children.end(), child);
		if (it != _children.end())
		{
			_children.erase(it);
		}
	}

	void Transform::Update()
	{
		_world = BuildTransform();
		if (_parent)
		{
			_world = _parent->GetWorld() * _world;
		}

		for (Transform* child : _children)
		{
			child->Update();
		}
	}
	Mat4x4f Transform::BuildTransform() const
	{
		Mat4x4f world = Mat4x4f::CreateIdentity();
		world.SetScale(_scale);
		world = world * _rotation;
		world.SetTranslation(_position);

		return world;
	}

} // namespace sb


