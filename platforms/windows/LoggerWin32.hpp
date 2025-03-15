#pragma once

#include <string>
#include "common/Logger.hpp"

class LoggerWin32 : public Logger
{
	void print(eLogLevel, const char* const str) override;
	void print(eLogLevel, std::string str) override;
	void vprintf(eLogLevel, const char* const fmt, va_list argPtr) override;
	void printf(eLogLevel, const char* const fmt, ...) override;
};