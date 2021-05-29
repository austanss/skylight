#pragma once
#include "drivers/uart/serial.h"

#ifdef __DEBUG__
#define DEBUG_PRINT(x, y) serial_terminal()->put##y(x);
#endif

#ifndef NDEBUG
/*dont let the compiler optimize out my variable, please!*/
#define DBG_VAR volatile
/*static_assert is valid in C11 through gnu C extensions which are used by default in clang.*/
#define GLASS_ASSERT(cond) static_assert(cond, "Compiletime test failed");
#warning "file" __FILE__ "is compiling with compiletime and runtime debugging features enabled! Expect lower performance".

#else

#define DBG_VAR /*a comment.*/
#define GLASS_ASSERT(cond) /*another comment*/

#endif
