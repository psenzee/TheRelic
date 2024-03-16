#pragma once

#include <array>

#define A2 std::array<T, 2>
#define A3 std::array<T, 3>
#define A4 std::array<T, 4>
#define AN std::array<T, N>

#define FTN template <typename T, size_t N> constexpr

FTN A4 argb(const AN &a) { return A4 { a[3], a[1], a[2], a[0] }; }
FTN A4 arbg(const AN &a) { return A4 { a[3], a[2], a[1], a[0] }; }
FTN A4 agrb(const AN &a) { return A4 { a[3], a[0], a[2], a[1] }; }
FTN A4 agbr(const AN &a) { return A4 { a[3], a[2], a[0], a[1] }; }
FTN A4 abrg(const AN &a) { return A4 { a[3], a[0], a[1], a[2] }; }
FTN A4 abgr(const AN &a) { return A4 { a[3], a[1], a[0], a[2] }; }
FTN A4 ragb(const AN &a) { return A4 { a[0], a[3], a[1], a[2] }; }
FTN A4 rabg(const AN &a) { return A4 { a[0], a[3], a[2], a[1] }; }
FTN A4 rgab(const AN &a) { return A4 { a[0], a[1], a[3], a[2] }; }
FTN A4 rgba(const AN &a) { return A4 { a[0], a[1], a[2], a[3] }; }
FTN A4 rbag(const AN &a) { return A4 { a[0], a[2], a[3], a[1] }; }
FTN A4 rbga(const AN &a) { return A4 { a[0], a[2], a[1], a[3] }; }
FTN A4 garb(const AN &a) { return A4 { a[1], a[3], a[0], a[2] }; }
FTN A4 gabr(const AN &a) { return A4 { a[1], a[3], a[2], a[0] }; }
FTN A4 grab(const AN &a) { return A4 { a[1], a[0], a[3], a[2] }; }
FTN A4 grba(const AN &a) { return A4 { a[1], a[0], a[2], a[3] }; }
FTN A4 gbar(const AN &a) { return A4 { a[1], a[2], a[3], a[0] }; }
FTN A4 gbra(const AN &a) { return A4 { a[1], a[2], a[0], a[3] }; }
FTN A4 barg(const AN &a) { return A4 { a[2], a[3], a[0], a[1] }; }
FTN A4 bagr(const AN &a) { return A4 { a[2], a[3], a[1], a[0] }; }
FTN A4 brag(const AN &a) { return A4 { a[2], a[0], a[3], a[1] }; }
FTN A4 brga(const AN &a) { return A4 { a[2], a[0], a[1], a[3] }; }
FTN A4 bgar(const AN &a) { return A4 { a[2], a[1], a[3], a[0] }; }
FTN A4 bgra(const AN &a) { return A4 { a[2], a[1], a[0], a[3] }; }

FTN A3 rbg(const AN &a)  { return A3 { a[0], a[2], a[1] }; }
FTN A3 bgr(const AN &a)  { return A3 { a[2], a[1], a[0] }; }
FTN A3 grb(const AN &a)  { return A3 { a[1], a[0], a[2] }; }
FTN A3 rgb(const AN &a)  { return A3 { a[0], a[1], a[2] }; }
FTN A3 gbr(const AN &a)  { return A3 { a[1], a[2], a[0] }; }
FTN A3 brg(const AN &a)  { return A3 { a[2], a[0], a[1] }; }

FTN A2 rb(const AN &a)   { return A2 { a[0], a[2] }; }
FTN A2 bg(const AN &a)   { return A2 { a[2], a[1] }; }
FTN A2 gr(const AN &a)   { return A2 { a[1], a[0] }; }
FTN A2 rg(const AN &a)   { return A2 { a[0], a[1] }; }
FTN A2 gb(const AN &a)   { return A2 { a[1], a[2] }; }
FTN A2 br(const AN &a)   { return A2 { a[2], a[0] }; }

FTN T  r(const AN &a)    { return a[0]; }
FTN T  g(const AN &a)    { return a[1]; }
FTN T  b(const AN &a)    { return a[2]; }
FTN T  a(const AN &a)    { return a[3]; }

#undef FTN

#undef AN
#undef A4
#undef A3
#undef A2
