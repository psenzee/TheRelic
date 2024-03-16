def list_a(count, start = 0):
    values = []
    for i in range(0, count):
        v = i + start
        values.append(f'a[{v}]')
    return ', '.join(values)

def list_fn(fn, count, start = 0):
    values = []
    for i in range(0, count):
        v = i + start
        values.append(f'{fn}(a[{v}])')
    return ', '.join(values)

def list_ai(count, start = 0):
    values = []
    for i in range(0, count):
        v = i + start
        values.append(f'a[i[{v}]]')
    return ', '.join(values)
    
def list_a_rev(count, start = 0):
    values = []
    v = (count - 1) + start
    for i in range(0, count):
        values.append(f'a[{v}]')
        v -= 1
    return ', '.join(values)

def generate_prefix_op_a(op, count):
    values = []
    for i in range(0, count):
        values.append(f'{op}a[{i}]')
    return ', '.join(values)

def generate_infix_op_a_a(op, count):
    values = []
    for i in range(0, count):
        values.append(f'a[{i}] {op} T(b[{i}])')
    return ', '.join(values)
    
def generate_infix_op_a_u(op, count):
    values = []
    for i in range(0, count):
        values.append(f'a[{i}] {op} T(b)')
    return ', '.join(values)
    
def generate_infix_op_u_a(op, count):
    values = []
    for i in range(0, count):
        values.append(f'T(a) {op} b[{i}]')
    return ', '.join(values)
    
def generate_inplace_op_a_a(op, count):
    values = []
    for i in range(0, count):
        values.append(f'a[{i}] {op} T(b[{i}])')
    return '; '.join(values)
    
def generate_inplace_op_a_u(op, count):
    values = []
    for i in range(0, count):
        values.append(f'a[{i}] {op} T(b)')
    return '; '.join(values)
    
def generate_indices(count):
    values = []
    for i in range(0, count):
        values.append(f'T({i})')
    return ', '.join(values)
    
def generate_sum(count):
    values = []
    for i in range(0, count):
        values.append(f'a[{i}]')
    return ' + '.join(values)
    
def generate_dot(count):
    values = []
    for i in range(0, count):
        values.append(f'a[{i}] * T(b[{i}])')
    return ' + '.join(values)
    
def generate_dot_self(count):
    values = []
    for i in range(0, count):
        values.append(f'a[{i}] * a[{i}]')
    return ' + '.join(values)
    
def generate_print(count):
    values = []
    for i in range(0, count):
        values.append(f'a[{i}]')
    return ' << " " << '.join(values)
    
def add_infix_op(map, op, count):
    map[f'A{op}A'] = generate_infix_op_a_a(op, count)
    map[f'A{op}U'] = generate_infix_op_a_u(op, count)
    map[f'U{op}A'] = generate_infix_op_u_a(op, count)
    
def add_inplace_op(map, op, count):
    map[f'A{op}A'] = generate_inplace_op_a_a(op, count)
    map[f'A{op}U'] = generate_inplace_op_a_u(op, count)

def generate(count):
    map = {'N' : count, 'N-1' : count - 1, 'N+1' : count + 1}
    map['-A'] = generate_prefix_op_a('-', count)
    map['~A'] = generate_prefix_op_a('~', count)    
    map['list_n'] = list_a(count)
    map['list_ai'] = list_ai(count)    
    map['list_n_rev'] = list_a_rev(count)
    map['list_n-1'] = list_a(count - 1)
    map['list_n-1+1'] = list_a(count - 1, 1)    
    map['list_n-1_rev'] = list_a_rev(count - 1)
    map['list_int'] = list_fn('int', count)
    map['list_floor'] = list_fn('std::floor', count)    
    map['indices'] = generate_indices(count)
    map['sum'] = generate_sum(count)
    map['dot'] = generate_dot(count)
    map['dot_self'] = generate_dot_self(count)
    map['print'] = generate_print(count)
    for op in ['+', '*', '-', '/', '%', '|', '&', '^']:
        add_infix_op(map, op, count)
    for op in ['+=', '*=', '-=', '/=', '%=', '|=', '&=', '^=']:
        add_inplace_op(map, op, count)

    template = """
#pragma once

#include <array>
#include <cmath>
#include <iostream>

#define _AT   std::array<T, [% N %]>
#define _AI   std::array<int, [% N %]>
#define _ASZ  std::array<size_t, [% N %]>
#define _AT_1 std::array<T, [% N-1 %]>
#define _AU   std::array<U, [% N %]>

template <typename T, typename U> constexpr _AT  operator-(const _AT &a)                  { return _AT({ [% -A %] }); }
template <typename T, typename U> constexpr _AT  operator~(const _AT &a)                  { return _AT({ [% ~A %] }); }

template <typename T, typename U> constexpr _AT  operator+(const _AT &a, const _AU &b)    { return _AT({ [% A+A %] }); }
template <typename T, typename U> constexpr _AT  operator*(const _AT &a, const _AU &b)    { return _AT({ [% A*A %] }); }
template <typename T, typename U> constexpr _AT  operator-(const _AT &a, const _AU &b)    { return _AT({ [% A-A %] }); }
template <typename T, typename U> constexpr _AT  operator/(const _AT &a, const _AU &b)    { return _AT({ [% A/A %] }); }
template <typename T, typename U> constexpr _AT  operator%(const _AT &a, const _AU &b)    { return _AT({ [% A%A %] }); }
template <typename T, typename U> constexpr _AT  operator|(const _AT &a, const _AU &b)    { return _AT({ [% A|A %] }); }
template <typename T, typename U> constexpr _AT  operator&(const _AT &a, const _AU &b)    { return _AT({ [% A&A %] }); }
template <typename T, typename U> constexpr _AT  operator^(const _AT &a, const _AU &b)    { return _AT({ [% A^A %] }); }

template <typename T, typename U> constexpr _AT  operator+(const _AT &a, const U &b)      { return _AT({ [% A+U %] }); }
template <typename T, typename U> constexpr _AT  operator*(const _AT &a, const U &b)      { return _AT({ [% A*U %] }); }
template <typename T, typename U> constexpr _AT  operator-(const _AT &a, const U &b)      { return _AT({ [% A-U %] }); }
template <typename T, typename U> constexpr _AT  operator/(const _AT &a, const U &b)      { return _AT({ [% A/U %] }); }
template <typename T, typename U> constexpr _AT  operator%(const _AT &a, const U &b)      { return _AT({ [% A%U %] }); }
template <typename T, typename U> constexpr _AT  operator|(const _AT &a, const U &b)      { return _AT({ [% A|U %] }); }
template <typename T, typename U> constexpr _AT  operator&(const _AT &a, const U &b)      { return _AT({ [% A&U %] }); }
template <typename T, typename U> constexpr _AT  operator^(const _AT &a, const U &b)      { return _AT({ [% A^U %] }); }

template <typename T, typename U> constexpr _AT  operator+(const U &a, const _AT &b)      { return _AT({ [% U+A %] }); }
template <typename T, typename U> constexpr _AT  operator*(const U &a, const _AT &b)      { return _AT({ [% U*A %] }); }
template <typename T, typename U> constexpr _AT  operator-(const U &a, const _AT &b)      { return _AT({ [% U-A %] }); }
template <typename T, typename U> constexpr _AT  operator/(const U &a, const _AT &b)      { return _AT({ [% U/A %] }); }
template <typename T, typename U> constexpr _AT  operator%(const U &a, const _AT &b)      { return _AT({ [% U%A %] }); }
template <typename T, typename U> constexpr _AT  operator|(const U &a, const _AT &b)      { return _AT({ [% U|A %] }); }
template <typename T, typename U> constexpr _AT  operator&(const U &a, const _AT &b)      { return _AT({ [% U&A %] }); }
template <typename T, typename U> constexpr _AT  operator^(const U &a, const _AT &b)      { return _AT({ [% U^A %] }); }

template <typename T, typename U> constexpr _AT &operator+=(_AT &a, const U &b)           { [% A+=U %]; return a; }
template <typename T, typename U> constexpr _AT &operator*=(_AT &a, const U &b)           { [% A*=U %]; return a; }
template <typename T, typename U> constexpr _AT &operator-=(_AT &a, const U &b)           { [% A-=U %]; return a; }
template <typename T, typename U> constexpr _AT &operator/=(_AT &a, const U &b)           { [% A/=U %]; return a; }
template <typename T, typename U> constexpr _AT &operator%=(_AT &a, const U &b)           { [% A%=U %]; return a; }
template <typename T, typename U> constexpr _AT &operator|=(_AT &a, const U &b)           { [% A|=U %]; return a; }
template <typename T, typename U> constexpr _AT &operator&=(_AT &a, const U &b)           { [% A&=U %]; return a; }
template <typename T, typename U> constexpr _AT &operator^=(_AT &a, const U &b)           { [% A^=U %]; return a; }

template <typename T, typename U> constexpr _AT &operator+=(_AT &a, const _AU &b)         { [% A+=A %]; return a; }
template <typename T, typename U> constexpr _AT &operator*=(_AT &a, const _AU &b)         { [% A*=A %]; return a; }
template <typename T, typename U> constexpr _AT &operator-=(_AT &a, const _AU &b)         { [% A-=A %]; return a; }
template <typename T, typename U> constexpr _AT &operator/=(_AT &a, const _AU &b)         { [% A/=A %]; return a; }
template <typename T, typename U> constexpr _AT &operator%=(_AT &a, const _AU &b)         { [% A%=A %]; return a; }
template <typename T, typename U> constexpr _AT &operator|=(_AT &a, const _AU &b)         { [% A|=A %]; return a; }
template <typename T, typename U> constexpr _AT &operator&=(_AT &a, const _AU &b)         { [% A&=A %]; return a; }
template <typename T, typename U> constexpr _AT &operator^=(_AT &a, const _AU &b)         { [% A^=A %]; return a; }

template <typename T> constexpr std::ostream &operator<<(std::ostream &os, const _AT &a)  { return os << "[" << [% print %] << "]"; }

template <typename T>             constexpr T     indices(const _AT &unused)              { return _AT({ [% indices %] }); }
template <typename T>             constexpr T     sum(const _AT &a)                       { return [% sum %]; }
template <typename T, typename U> constexpr T     dot(const _AT &a, const _AU &b)         { return [% dot %]; }
template <typename T>             constexpr T     dot_self(const _AT &a)                  { return [% dot_self %]; }
template <typename T>             constexpr T     magnitude(const _AT &a)                 { return sqrt(dot_self(a)); }
template <typename T, typename U> constexpr T     distance_sq(const _AT &a, const _AU &b) { return dot_self(a - b); }
template <typename T, typename U> constexpr T     distance(const _AT &a, const _AU &b)    { return sqrt(distance_sq(a, b)); }
template <typename T>             constexpr _AT   normal(const _AT &a)                    { return a / magnitude(a); }
template <typename T>             constexpr _AI   ints(const _AT &a)                      { return _AI({ [% list_int %] }); }
template <typename T>             constexpr _AT   floor(const _AT &a)                     { return _AT({ [% list_floor %] }); }
template <typename T, typename U> constexpr _AT   from(const _AT_1 &a, U last = U(0))     { return _AT({ [% list_n-1 %], T(last) }); }
template <typename T>             constexpr _AT_1 rchop(const _AT &a)                     { return _AT_1({ [% list_n-1 %] }); }
template <typename T>             constexpr _AT_1 lchop(const _AT &a)                     { return _AT_1({ [% list_n-1+1 %] }); }
template <typename T>             constexpr _AT   lrotate(const _AT &a)                   { return _AT({ [% list_n-1+1 %], a[0] }); }
template <typename T>             constexpr _AT   rrotate(const _AT &a)                   { return _AT({ a[[% N-1 %]], [% list_n-1 %] }); }
template <typename T>             constexpr _AT   reorder(const _AT &a, const _ASZ &i)    { return _AT({ [% list_ai %] }); }
template <typename T>             constexpr _AT   reverse(const _AT &a)                   { return _AT({ [% list_n_rev %] }); }

#undef _AT
#undef _AI
#undef _ASZ
#undef _AT_1
#undef _AU
"""
    text = template
    for key in map.keys():
        k = "[% " + key + " %]"
        v = map[key]
        text = text.replace(k, str(v))
    return text
    
print(generate(8))
