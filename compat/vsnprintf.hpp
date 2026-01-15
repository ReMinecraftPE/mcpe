#pragma once

#include <stdio.h>
#ifndef vsnprintf
#include <stdarg.h>
extern int remcpe_vsnprintf(char *, size_t, const char *, va_list ap);
#define vsnprintf remcpe_vsnprintf
#endif
