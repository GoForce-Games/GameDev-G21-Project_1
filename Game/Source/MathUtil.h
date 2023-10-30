#pragma once
#include <algorithm>

//#define CLAMP(val,low,high) std::max(low,std::min(val,high))

#define LERP(a,b,f) ((a)*(1.0f-(f))+((b)*(f)))