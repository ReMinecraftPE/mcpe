#pragma once

#include <string>
#include "common/Logger.hpp"

class LoggerWindows : Logger
{
	void print(const char* const str) override;
	void print(std::string str) override;
	void vprintf(const char* const fmt, va_list argPtr) override;
	void printf(const char* const fmt, ...) override;
};