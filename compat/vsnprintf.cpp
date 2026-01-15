#include <stdio.h>

extern "C" {
	int remcpe_vsnprintf(char* buf, size_t size, const char* format, va_list ap)
	{
		return vsnprintf(buf, size, format, ap);
	}
}