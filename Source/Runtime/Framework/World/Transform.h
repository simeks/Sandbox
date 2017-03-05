// Copyright 2008-2014 Simon Ekström

#ifndef __FRAMEWORK_TRANSFORM_H__
#define __FRAMEWORK_TRANSFORM_H__

#include <Foundation/Math/Matrix4x4.h>
#include <Foundation/Math/Matrix3x3.h>
#include <Foundation/Math/Vec3.h>


namespace sb
{

	class SceneGraphManager;
	class SceneGraph;
	class Transform
	{
	public:
		Transform();
		~Transform();

		void SetLocalRotation(const Mat3x3f& rotation);
		void SetLocalPosition(const Vec3f& position);
		void SetLocalScale(const Vec3f& scale);

		const Mat3x3f& GetLocalRotation() const;
		const Vec3f& GetLocalPosition() const;
		const Vec3f& GetLocalScale() const;

		/// @brief Returns world transformation for this object.
		const Mat4x4f& GetWorld() const;

		Transform* GetParent();

		/// Detaches this node from any parent node.
		void Detach();

		void AttachChild(Transform* child);
		void DetachChild(Transform* child);


		void Update();

	private:
		Mat4x4f BuildTransform() const;

		Transform* _parent;
		vector<Transform*> _children;

		Mat3x3f _rotation;
		Vec3f _position;
		Vec3f _scale;

		Mat4x4f _world;

	};

} // namespace sb



#endif // __FRAMEWORK_TRANSFORM_H__
