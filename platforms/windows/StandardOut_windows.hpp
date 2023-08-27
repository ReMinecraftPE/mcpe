#pragma once

#include <string>
#include "common/StandardOut.hpp"

class StandardOut_windows : StandardOut
{
	void print(const char* const str) override;
	void print(std::string str) override;
	void vprintf(const char* const fmt, va_list argPtr) override;
	void printf(const char* const fmt, ...) override;
};