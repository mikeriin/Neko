#ifndef NEKO_H
#define NEKO_H



using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using i8 = char;
using i16 = short;
using i32 = int;
using i64 = long long;

using f32 = float;
using f64 = double;


template<typename T>
struct Allowed;

template<> struct Allowed<i32> {};
template<> struct Allowed<u32> {};
template<> struct Allowed<i64> {};
template<> struct Allowed<u64> {};
template<> struct Allowed<float> {};
template<> struct Allowed<double> {};


#include "utils/FileReader.h"



#endif // !NEKO_H
