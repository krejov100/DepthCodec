#pragma once
#include <iostream>
#include <map>
#include <algorithm>

namespace std
{
	template<typename T1, typename T2>
	std::ostream & operator<<(std::ostream &os, const std::pair<T1, T2> &v)
	{
		os << v.first << v.second;
		return os;
	}

	template<typename T1, typename T2>
	std::istream & operator>>(std::istream &is, std::pair<T1, T2> &v)
	{
		is >> v.first >> v.second;
		return is;
	}

	template<typename K, typename T>
	std::ostream & operator<<(std::ostream &os, const std::map<K, T> &v)
	{
		os << v.size() << " ";
		std::copy(v.begin(), v.end(), std::ostream_iterator<std::pair<K, T>>(os));
		return os;
	}

	template<typename K, typename T>
	std::istream & operator>>(std::istream &is, std::map<K, T> &v)
	{
		size_t size;
		is >> size;
		std::copy_n(std::istream_iterator<std::pair<K, T>>(is), size, std::inserter(v, v.begin()));
		return is;
	}
}