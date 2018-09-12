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

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        // note, version is always the latest when saving
        ar & mMin;
        ar & mMax;
    }

    friend class boost::serialization::access;
};