#pragma once

#include <array>

#define A2 std::array<T, 2>
#define A3 std::array<T, 3>
#define A4 std::array<T, 4>
#define AN std::array<T, N>

#define FTN template <typename T, size_t N> constexpr
#define FT  template <typename T> constexpr

FTN A4 wxyz(const AN &a)              { return A4 { a[3], a[1], a[2], a[0] }; }
FTN A4 wxzy(const AN &a)              { return A4 { a[3], a[2], a[1], a[0] }; }
FTN A4 wyxz(const AN &a)              { return A4 { a[3], a[0], a[2], a[1] }; }
FTN A4 wyzx(const AN &a)              { return A4 { a[3], a[2], a[0], a[1] }; }
FTN A4 wzxy(const AN &a)              { return A4 { a[3], a[0], a[1], a[2] }; }
FTN A4 wzyx(const AN &a)              { return A4 { a[3], a[1], a[0], a[2] }; }
FTN A4 xwyz(const AN &a)              { return A4 { a[0], a[3], a[1], a[2] }; }
FTN A4 xwzy(const AN &a)              { return A4 { a[0], a[3], a[2], a[1] }; }
FTN A4 xywz(const AN &a)              { return A4 { a[0], a[1], a[3], a[2] }; }
FTN A4 xyzw(const AN &a)              { return A4 { a[0], a[1], a[2], a[3] }; }
FTN A4 xzwy(const AN &a)              { return A4 { a[0], a[2], a[3], a[1] }; }
FTN A4 xzyw(const AN &a)              { return A4 { a[0], a[2], a[1], a[3] }; }
FTN A4 ywxz(const AN &a)              { return A4 { a[1], a[3], a[0], a[2] }; }
FTN A4 ywzx(const AN &a)              { return A4 { a[1], a[3], a[2], a[0] }; }
FTN A4 yxwz(const AN &a)              { return A4 { a[1], a[0], a[3], a[2] }; }
FTN A4 yxzw(const AN &a)              { return A4 { a[1], a[0], a[2], a[3] }; }
FTN A4 yzwx(const AN &a)              { return A4 { a[1], a[2], a[3], a[0] }; }
FTN A4 yzxw(const AN &a)              { return A4 { a[1], a[2], a[0], a[3] }; }
FTN A4 zwxy(const AN &a)              { return A4 { a[2], a[3], a[0], a[1] }; }
FTN A4 zwyx(const AN &a)              { return A4 { a[2], a[3], a[1], a[0] }; }
FTN A4 zxwy(const AN &a)              { return A4 { a[2], a[0], a[3], a[1] }; }
FTN A4 zxyw(const AN &a)              { return A4 { a[2], a[0], a[1], a[3] }; }
FTN A4 zywx(const AN &a)              { return A4 { a[2], a[1], a[3], a[0] }; }
FTN A4 zyxw(const AN &a)              { return A4 { a[2], a[1], a[0], a[3] }; }

FTN A3 xzy(const AN &a)               { return A3 { a[0], a[2], a[1] }; }
FTN A3 zyx(const AN &a)               { return A3 { a[2], a[1], a[0] }; }
FTN A3 yxz(const AN &a)               { return A3 { a[1], a[0], a[2] }; }
FTN A3 xyz(const AN &a)               { return A3 { a[0], a[1], a[2] }; }
FTN A3 yzx(const AN &a)               { return A3 { a[1], a[2], a[0] }; }
FTN A3 zxy(const AN &a)               { return A3 { a[2], a[0], a[1] }; }

FTN A2 xz(const AN &a)                { return A2 { a[0], a[2] }; }
FTN A2 zy(const AN &a)                { return A2 { a[2], a[1] }; }
FTN A2 yx(const AN &a)                { return A2 { a[1], a[0] }; }
FTN A2 xy(const AN &a)                { return A2 { a[0], a[1] }; }
FTN A2 yz(const AN &a)                { return A2 { a[1], a[2] }; }
FTN A2 zx(const AN &a)                { return A2 { a[2], a[0] }; }

FTN T  x(const AN &a)                 { return a[0]; }
FTN T  y(const AN &a)                 { return a[1]; }
FTN T  z(const AN &a)                 { return a[2]; }
FTN T  w(const AN &a)                 { return a[3]; }

FTN A4 _3012(const AN &a)             { return A4 { a[3], a[1], a[2], a[0] }; }
FTN A4 _3021(const AN &a)             { return A4 { a[3], a[2], a[1], a[0] }; }
FTN A4 _3102(const AN &a)             { return A4 { a[3], a[0], a[2], a[1] }; }
FTN A4 _3120(const AN &a)             { return A4 { a[3], a[2], a[0], a[1] }; }
FTN A4 _3201(const AN &a)             { return A4 { a[3], a[0], a[1], a[2] }; }
FTN A4 _3210(const AN &a)             { return A4 { a[3], a[1], a[0], a[2] }; }
FTN A4 _0312(const AN &a)             { return A4 { a[0], a[3], a[1], a[2] }; }
FTN A4 _0321(const AN &a)             { return A4 { a[0], a[3], a[2], a[1] }; }
FTN A4 _0132(const AN &a)             { return A4 { a[0], a[1], a[3], a[2] }; }
FTN A4 _0123(const AN &a)             { return A4 { a[0], a[1], a[2], a[3] }; }
FTN A4 _0231(const AN &a)             { return A4 { a[0], a[2], a[3], a[1] }; }
FTN A4 _0213(const AN &a)             { return A4 { a[0], a[2], a[1], a[3] }; }
FTN A4 _1302(const AN &a)             { return A4 { a[1], a[3], a[0], a[2] }; }
FTN A4 _1320(const AN &a)             { return A4 { a[1], a[3], a[2], a[0] }; }
FTN A4 _1032(const AN &a)             { return A4 { a[1], a[0], a[3], a[2] }; }
FTN A4 _1023(const AN &a)             { return A4 { a[1], a[0], a[2], a[3] }; }
FTN A4 _1230(const AN &a)             { return A4 { a[1], a[2], a[3], a[0] }; }
FTN A4 _1203(const AN &a)             { return A4 { a[1], a[2], a[0], a[3] }; }
FTN A4 _2301(const AN &a)             { return A4 { a[2], a[3], a[0], a[1] }; }
FTN A4 _2310(const AN &a)             { return A4 { a[2], a[3], a[1], a[0] }; }
FTN A4 _2031(const AN &a)             { return A4 { a[2], a[0], a[3], a[1] }; }
FTN A4 _2013(const AN &a)             { return A4 { a[2], a[0], a[1], a[3] }; }
FTN A4 _2130(const AN &a)             { return A4 { a[2], a[1], a[3], a[0] }; }
FTN A4 _2103(const AN &a)             { return A4 { a[2], a[1], a[0], a[3] }; }

FTN A3 _021(const AN &a)              { return A3 { a[0], a[2], a[1] }; }
FTN A3 _210(const AN &a)              { return A3 { a[2], a[1], a[0] }; }
FTN A3 _102(const AN &a)              { return A3 { a[1], a[0], a[2] }; }
FTN A3 _012(const AN &a)              { return A3 { a[0], a[1], a[2] }; }
FTN A3 _120(const AN &a)              { return A3 { a[1], a[2], a[0] }; }
FTN A3 _201(const AN &a)              { return A3 { a[2], a[0], a[1] }; }

FTN A2 _02(const AN &a)               { return A2 { a[0], a[2] }; }
FTN A2 _21(const AN &a)               { return A2 { a[2], a[1] }; }
FTN A2 _10(const AN &a)               { return A2 { a[1], a[0] }; }
FTN A2 _01(const AN &a)               { return A2 { a[0], a[1] }; }
FTN A2 _12(const AN &a)               { return A2 { a[1], a[2] }; }
FTN A2 _20(const AN &a)               { return A2 { a[2], a[0] }; }

#undef FT
#undef FTN

#undef AN
#undef A4
#undef A3
#undef A2
