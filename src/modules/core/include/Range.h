#pragma once
#define NOMINMAX
#include <iostream>
#include <algorithm>

template<typename T>
class Range
{
	T mMin;
	T mMax;
public:
	Range(){};
	Range(T v):mMin(v), mMax(v) {}
	Range(T min, T max):mMin(min), mMax(max) {}
	/// makes a new range that is the union of ranges
	Range(const std::vector<Range>& ranges)
	{
		if (ranges.empty())
			return;
		mMin = ranges[0].mMin;
		mMax = ranges[0].mMax;
		for (auto& r : ranges)
		{
			mMin = (std::min)(mMin, r.mMin);
			mMax = (std::max)(mMax, r.mMin);
		}
	}

	T getMin(){
		return mMin;
	}

	T getMax()
	{
		return mMax;
	}

	friend std::ostream &operator<<(std::ostream &, const Range<T> &);
	friend std::istream &operator>>(std::istream &, Range<T> &);
};

template<typename T>
std::ostream operator<<(std::ostream &os, const Range<T> &v)
{
	os << v.mMin;
	os << v.mMax;
	return os;
}

template<typename T>
std::ostream operator>>(std::istream &is, Range<T> &v)
{
	is >> v.mMin;
	is >> v.mMax;
	return is;
}