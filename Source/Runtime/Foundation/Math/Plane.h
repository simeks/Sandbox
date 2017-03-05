// Copyright 2008-2014 Simon Ekström

#ifndef _MATH_PLANE_H
#define _MATH_PLANE_H


namespace sb
{

	template<typename T> class Vec3;
	template<typename T>
	class Plane
	{
	public:
		Vec3<T> n;	// Normal
		T d;	// Distance


		//-------------------------------------------------------------------------------
		// Constructors

		Plane();
		explicit Plane(const Vec3<T>& normal, const T& distance);

		//-------------------------------------------------------------------------------

		void Set(const Vec3<T>& normal, const T& distance);
		void Set(const T& x, const T& y, const T& z, const T& distance);

		/// @brief Creates a plane from 3 points 
		void SetPlane(const Vec3<T>& p1, const Vec3<T>& p2, const Vec3<T>& p3);

		//-------------------------------------------------------------------------------

		/// @brief Normalizes the plane
		void Normalize();

		/// @brief Returns the Distance between a plane and the given point
		T Distance(const Vec3<T>& p) const;


		//-------------------------------------------------------------------------------
		// Operators

		Plane<T> operator-() const;

		bool operator==(const Plane& v) const;
		bool operator!=(const Plane& v) const;

		//-------------------------------------------------------------------------------

		/// @brief Creates a plane from 3 points
		static INLINE Plane<T> CreatePlane(const Vec3<T>& p1, const Vec3<T>& p2, const Vec3<T>& p3);

		//-------------------------------------------------------------------------------
	};

	typedef Plane<float> Planef;

	//-------------------------------------------------------------------------------
	template<typename T>
	Plane<T>::Plane() : n(Vec3<T>::ONE), d((T)0)
	{
	}

	template<typename T>
	Plane<T>::Plane(const Vec3<T>& normal, const T& distance) : n(normal), d(distance)
	{
	}

	//-------------------------------------------------------------------------------

	template<typename T>
	void Plane<T>::Set(const Vec3<T>& normal, const T& distance)
	{
		n = normal;
		d = distance;
	}
	template<typename T>
	void Plane<T>::Set(const T& x, const T& y, const T& z, const T& distance)
	{
		n = Vec3<T>(x, y, z);
		d = distance;
	}
	template<typename T>
	void Plane<T>::SetPlane(const Vec3<T>& p1, const Vec3<T>& p2, const Vec3<T>& p3)
	{
		n = (p2 - p1).Cross((p3 - p1));
		d = -(n.Dot(p1));
	}

	//-------------------------------------------------------------------------------

	template<typename T>
	void Plane<T>::Normalize()
	{
		if (n.Length() > 0.0f)
		{
			float inv_l = 1.0f / n.Length();
			n.Normalize();
			d *= inv_l;
		}
	}

	template<typename T>
	T Plane<T>::Distance(const Vec3<T>& p) const
	{
		return n.x * p.x + n.y * p.y + n.z * p.z + d;
	}



	//-------------------------------------------------------------------------------

	template<typename T>
	Plane<T> Plane<T>::operator-() const
	{
		return Plane<T>(-n, -d);
	}

	template<typename T>
	bool Plane<T>::operator==(const Plane& v) const
	{
		return ((n.x == v.n.x) && (n.y == v.n.y) && (n.z == v.n.z) && (d == v.d));
	}
	template<typename T>
	bool Plane<T>::operator!=(const Plane& v) const
	{
		return !((n.x == v.n.x) && (n.y == v.n.y) && (n.z == v.n.z) && (d == v.d));
	}

	//-------------------------------------------------------------------------------

	template<typename T>
	INLINE Plane<T> Plane<T>::CreatePlane(const Vec3<T>& p1, const Vec3<T>& p2, const Vec3<T>& p3)
	{
		Plane<T> plane;
		plane.SetPlane(p1, p2, p3);
		return plane;
	}
	//-------------------------------------------------------------------------------


} // namespace sb


#endif // _MATH_PLANE_H