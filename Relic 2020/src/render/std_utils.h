#pragma once

#include <vector>
#include <span>

template <typename T>
inline void overwrite_append(std::span<T> s, std::vector<T> &v, size_t at = 0)
{
    v.resize(std::max(v.size(), s.size() + at));
    std::copy(s.begin(), s.end(), v.begin() + at);
}

template <typename T>
inline void overwrite_append(std::span<const T> s, std::vector<T> &v, size_t at = 0)
{
    v.resize(std::max(v.size(), s.size() + at));
    std::copy(s.begin(), s.end(), v.begin() + at);
}

template <typename T>
inline void append(std::span<T> s, std::vector<T> &v)
{
    overwrite_append(s, v, v.size());
}

template <typename T>
inline void append(std::span<const T> s, std::vector<T> &v)
{
    overwrite_append(s, v, v.size());
}

