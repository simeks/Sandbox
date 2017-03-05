// Copyright 2008-2014 Simon Ekström

#ifndef _MATH_RECTANGLE_H
#define _MATH_RECTANGLE_H


namespace sb
{

	template<typename T>
	class Rect
	{
	public:
		T left, top, right, bottom;

		Rect()
		{
			left = top = right = bottom = (T)0;
		}
		explicit Rect(const T& l, const T& t, const T& r, const T& b)
			: left(l), top(t), right(r), bottom(b)
		{
		}

		T Width() const
		{
			return right - left;
		}
		T Height() const
		{
			return bottom - top;
		}

	};

	typedef Rect<float> Rectf;

} // namespace sb



#endif // _MATH_RECTANGLE_H