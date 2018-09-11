#pragma once
#include <iostream>

template<typename T>
struct istream_iterator_count : public std::istream_iterator<T>
{
	istream_iterator_count(size_t c)
		:std::istream_iterator<T>()
		, count(c)
	{
	}
	istream_iterator_count(std::istream& str)
		:std::istream_iterator<T>(str)
		, count(0)
	{
	}

	bool operator!=(istream_iterator_count const& rhs) const
	{
		return (count != rhs.count) && (dynamic_cast<std::istream_iterator<T> const&>(*this) != rhs);
	}
	T operator*()
	{
		++count;
		return std::istream_iterator<T>::operator*();
	}

private:
	size_t  count;
};
