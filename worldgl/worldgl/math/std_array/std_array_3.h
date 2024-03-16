#pragma once

#include <array>
#include <cmath>
#include <iostream>

#define _AT   std::array<T, 3>
#define _AI   std::array<int, 3>
#define _ASZ  std::array<size_t, 3>
#define _AT_1 std::array<T, 2>
#define _AU   std::array<U, 3>

template <typename T, typename U> constexpr _AT  operator-(const _AT &a)                  { return _AT({ -a[0], -a[1], -a[2] }); }
template <typename T, typename U> constexpr _AT  operator~(const _AT &a)                  { return _AT({ ~a[0], ~a[1], ~a[2] }); }

template <typename T, typename U> constexpr _AT  operator+(const _AT &a, const _AU &b)    { return _AT({ a[0] + T(b[0]), a[1] + T(b[1]), a[2] + T(b[2]) }); }
template <typename T, typename U> constexpr _AT  operator*(const _AT &a, const _AU &b)    { return _AT({ a[0] * T(b[0]), a[1] * T(b[1]), a[2] * T(b[2]) }); }
template <typename T, typename U> constexpr _AT  operator-(const _AT &a, const _AU &b)    { return _AT({ a[0] - T(b[0]), a[1] - T(b[1]), a[2] - T(b[2]) }); }
template <typename T, typename U> constexpr _AT  operator/(const _AT &a, const _AU &b)    { return _AT({ a[0] / T(b[0]), a[1] / T(b[1]), a[2] / T(b[2]) }); }
template <typename T, typename U> constexpr _AT  operator%(const _AT &a, const _AU &b)    { return _AT({ a[0] % T(b[0]), a[1] % T(b[1]), a[2] % T(b[2]) }); }
template <typename T, typename U> constexpr _AT  operator|(const _AT &a, const _AU &b)    { return _AT({ a[0] | T(b[0]), a[1] | T(b[1]), a[2] | T(b[2]) }); }
template <typename T, typename U> constexpr _AT  operator&(const _AT &a, const _AU &b)    { return _AT({ a[0] & T(b[0]), a[1] & T(b[1]), a[2] & T(b[2]) }); }
template <typename T, typename U> constexpr _AT  operator^(const _AT &a, const _AU &b)    { return _AT({ a[0] ^ T(b[0]), a[1] ^ T(b[1]), a[2] ^ T(b[2]) }); }

template <typename T, typename U> constexpr _AT  operator+(const _AT &a, const U &b)      { return _AT({ a[0] + T(b), a[1] + T(b), a[2] + T(b) }); }
template <typename T, typename U> constexpr _AT  operator*(const _AT &a, const U &b)      { return _AT({ a[0] * T(b), a[1] * T(b), a[2] * T(b) }); }
template <typename T, typename U> constexpr _AT  operator-(const _AT &a, const U &b)      { return _AT({ a[0] - T(b), a[1] - T(b), a[2] - T(b) }); }
template <typename T, typename U> constexpr _AT  operator/(const _AT &a, const U &b)      { return _AT({ a[0] / T(b), a[1] / T(b), a[2] / T(b) }); }
template <typename T, typename U> constexpr _AT  operator%(const _AT &a, const U &b)      { return _AT({ a[0] % T(b), a[1] % T(b), a[2] % T(b) }); }
template <typename T, typename U> constexpr _AT  operator|(const _AT &a, const U &b)      { return _AT({ a[0] | T(b), a[1] | T(b), a[2] | T(b) }); }
template <typename T, typename U> constexpr _AT  operator&(const _AT &a, const U &b)      { return _AT({ a[0] & T(b), a[1] & T(b), a[2] & T(b) }); }
template <typename T, typename U> constexpr _AT  operator^(const _AT &a, const U &b)      { return _AT({ a[0] ^ T(b), a[1] ^ T(b), a[2] ^ T(b) }); }

template <typename T, typename U> constexpr _AT  operator+(const U &a, const _AT &b)      { return _AT({ T(a) + b[0], T(a) + b[1], T(a) + b[2] }); }
template <typename T, typename U> constexpr _AT  operator*(const U &a, const _AT &b)      { return _AT({ T(a) * b[0], T(a) * b[1], T(a) * b[2] }); }
template <typename T, typename U> constexpr _AT  operator-(const U &a, const _AT &b)      { return _AT({ T(a) - b[0], T(a) - b[1], T(a) - b[2] }); }
template <typename T, typename U> constexpr _AT  operator/(const U &a, const _AT &b)      { return _AT({ T(a) / b[0], T(a) / b[1], T(a) / b[2] }); }
template <typename T, typename U> constexpr _AT  operator%(const U &a, const _AT &b)      { return _AT({ T(a) % b[0], T(a) % b[1], T(a) % b[2] }); }
template <typename T, typename U> constexpr _AT  operator|(const U &a, const _AT &b)      { return _AT({ T(a) | b[0], T(a) | b[1], T(a) | b[2] }); }
template <typename T, typename U> constexpr _AT  operator&(const U &a, const _AT &b)      { return _AT({ T(a) & b[0], T(a) & b[1], T(a) & b[2] }); }
template <typename T, typename U> constexpr _AT  operator^(const U &a, const _AT &b)      { return _AT({ T(a) ^ b[0], T(a) ^ b[1], T(a) ^ b[2] }); }

template <typename T, typename U> constexpr _AT &operator+=(_AT &a, const U &b)           { a[0] += T(b); a[1] += T(b); a[2] += T(b); return a; }
template <typename T, typename U> constexpr _AT &operator*=(_AT &a, const U &b)           { a[0] *= T(b); a[1] *= T(b); a[2] *= T(b); return a; }
template <typename T, typename U> constexpr _AT &operator-=(_AT &a, const U &b)           { a[0] -= T(b); a[1] -= T(b); a[2] -= T(b); return a; }
template <typename T, typename U> constexpr _AT &operator/=(_AT &a, const U &b)           { a[0] /= T(b); a[1] /= T(b); a[2] /= T(b); return a; }
template <typename T, typename U> constexpr _AT &operator%=(_AT &a, const U &b)           { a[0] %= T(b); a[1] %= T(b); a[2] %= T(b); return a; }
template <typename T, typename U> constexpr _AT &operator|=(_AT &a, const U &b)           { a[0] |= T(b); a[1] |= T(b); a[2] |= T(b); return a; }
template <typename T, typename U> constexpr _AT &operator&=(_AT &a, const U &b)           { a[0] &= T(b); a[1] &= T(b); a[2] &= T(b); return a; }
template <typename T, typename U> constexpr _AT &operator^=(_AT &a, const U &b)           { a[0] ^= T(b); a[1] ^= T(b); a[2] ^= T(b); return a; }

template <typename T, typename U> constexpr _AT &operator+=(_AT &a, const _AU &b)         { a[0] += T(b[0]); a[1] += T(b[1]); a[2] += T(b[2]); return a; }
template <typename T, typename U> constexpr _AT &operator*=(_AT &a, const _AU &b)         { a[0] *= T(b[0]); a[1] *= T(b[1]); a[2] *= T(b[2]); return a; }
template <typename T, typename U> constexpr _AT &operator-=(_AT &a, const _AU &b)         { a[0] -= T(b[0]); a[1] -= T(b[1]); a[2] -= T(b[2]); return a; }
template <typename T, typename U> constexpr _AT &operator/=(_AT &a, const _AU &b)         { a[0] /= T(b[0]); a[1] /= T(b[1]); a[2] /= T(b[2]); return a; }
template <typename T, typename U> constexpr _AT &operator%=(_AT &a, const _AU &b)         { a[0] %= T(b[0]); a[1] %= T(b[1]); a[2] %= T(b[2]); return a; }
template <typename T, typename U> constexpr _AT &operator|=(_AT &a, const _AU &b)         { a[0] |= T(b[0]); a[1] |= T(b[1]); a[2] |= T(b[2]); return a; }
template <typename T, typename U> constexpr _AT &operator&=(_AT &a, const _AU &b)         { a[0] &= T(b[0]); a[1] &= T(b[1]); a[2] &= T(b[2]); return a; }
template <typename T, typename U> constexpr _AT &operator^=(_AT &a, const _AU &b)         { a[0] ^= T(b[0]); a[1] ^= T(b[1]); a[2] ^= T(b[2]); return a; }

template <typename T> constexpr std::ostream &operator<<(std::ostream &os, const _AT &a)  { return os << "[" << a[0] << " " << a[1] << " " << a[2] << "]"; }

template <typename T>             constexpr T     indices(const _AT &unused)              { return _AT({ T(0), T(1), T(2) }); }
template <typename T>             constexpr T     sum(const _AT &a)                       { return a[0] + a[1] + a[2]; }
template <typename T, typename U> constexpr T     dot(const _AT &a, const _AU &b)         { return a[0] * T(b[0]) + a[1] * T(b[1]) + a[2] * T(b[2]); }
template <typename T>             constexpr T     dot_self(const _AT &a)                  { return a[0] * a[0] + a[1] * a[1] + a[2] * a[2]; }
template <typename T>             constexpr T     magnitude(const _AT &a)                 { return std::sqrt(dot_self(a)); }
template <typename T, typename U> constexpr T     distance_sq(const _AT &a, const _AU &b) { return dot_self(a - b); }
template <typename T, typename U> constexpr T     distance(const _AT &a, const _AU &b)    { return std::sqrt(distance_sq(a, b)); }
template <typename T>             constexpr _AT   normal(const _AT &a)                    { return a / magnitude(a); }
template <typename T>             constexpr _AI   ints(const _AT &a)                      { return _AI({ int(a[0]), int(a[1]), int(a[2]) }); }
template <typename T>             constexpr _AT   floor(const _AT &a)                     { return _AT({ std::floor(a[0]), std::floor(a[1]), std::floor(a[2]) }); }
template <typename T, typename U> constexpr _AT   from(const _AT_1 &a, U last = U(0))     { return _AT({ a[0], a[1], T(last) }); }
template <typename T>             constexpr _AT_1 rchop(const _AT &a)                     { return _AT_1({ a[0], a[1] }); }
template <typename T>             constexpr _AT_1 lchop(const _AT &a)                     { return _AT_1({ a[1], a[2] }); }
template <typename T>             constexpr _AT   lrotate(const _AT &a)                   { return _AT({ a[1], a[2], a[0] }); }
template <typename T>             constexpr _AT   rrotate(const _AT &a)                   { return _AT({ a[2], a[0], a[1] }); }
template <typename T>             constexpr _AT   reorder(const _AT &a, const _ASZ &i)    { return _AT({ a[i[0]], a[i[1]], a[i[2]] }); }
template <typename T>             constexpr _AT   reverse(const _AT &a)                   { return _AT({ a[2], a[1], a[0] }); }

#undef _AT
#undef _AI
#undef _ASZ
#undef _AT_1
#undef _AU

