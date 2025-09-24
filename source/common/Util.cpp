/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Util.hpp"

const std::string Util::EMPTY_STRING = "";

std::string Util::stringTrim(const std::string& str, const std::string& filter, bool a4, bool a5)
{
	if (str.empty() || filter.empty())
		return "";

	// don't know what the hell this does. TODO: clean this crap up
	if (!a4 && !a5)
		return "";

	int startIndex = 0, endIndex = int(str.size()) - 1;

	// @TODO: Is this accurate?
	if (a4)
	{
		while (startIndex < endIndex && strchr(filter.c_str(), str[startIndex]))
		{
			startIndex++;
		}
	}
	if (a5)
	{
		while (startIndex < endIndex && strchr(filter.c_str(), str[endIndex]))
		{
			endIndex--;
		}
	}

	return str.substr(startIndex, endIndex + 1 - startIndex);
}

std::string Util::stringTrim(const std::string& str)
{
	return stringTrim(str, " \t\n\r", true, true);
}

std::string Util::vformat(const char *fmt, va_list argPtr)
{
	char str[1024];

	vsnprintf(str, sizeof(str), fmt, argPtr);

	return std::string(str);
}

std::string Util::format(const char *fmt, ...)
{
	std::string str;
	va_list argList;

	va_start(argList, fmt);
	str = vformat(fmt, argList);
	va_end(argList);

	return str;
}
