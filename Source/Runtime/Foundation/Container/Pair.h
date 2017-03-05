// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_PAIR_H__
#define __FOUNDATION_PAIR_H__

namespace sb
{

	template<typename T1, typename T2>
	class Pair
	{
	public:
		typedef T1 FirstType;
		typedef T2 SecondType;

		Pair()
			: first(), second()
		{
		}
		Pair(const T1& f, const T2& s)
			: first(f), second(s)
		{
		}
		Pair(const Pair& other)
			: first(other.first), second(other.second)
		{
		}
		~Pair()
		{
		}

		Pair<T1, T2>& operator=(const Pair& other)
		{
			first = other.first;
			second = other.second;
			return *this;
		}

		bool operator==(const Pair& other)
		{
			return (first == other.first && second == other.second);
		}
		bool operator!=(const Pair& other)
		{
			return (first != other.first || second != other.second);
		}

		bool operator<(const Pair& other) const
		{
			if (first < other.first)
			{
				return true;
			}
			else if (first > other.first)
			{
				return false;
			}

			return (second < other.second);
		}

		T1 first;
		T2 second;
	};


	/// Function class for extracting the first value from a Pair
	template<typename PairType>
	class SelectFirst
	{
	public:
		const typename PairType::FirstType& operator()(const PairType& pair) const
		{
			return pair.first;
		}
	};

} // namespace sb




#endif // __FOUNDATION_PAIR_H__

