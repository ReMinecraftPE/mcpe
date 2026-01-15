/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Util.hpp"
#include "compat/vsnprintf.hpp"

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

bool Util::isValidPath(const std::string& path)
{
	for (size_t i = 0; i < path.size(); i++)
	{
		switch (path.at(i))
		{
		case '\n':
		case '\r':
			return false;
		}
	}

	return true;
}

std::string Util::getFileName(const std::string& path)
{
	size_t namePos = 0;
	size_t nameSize = std::string::npos;

	size_t lastOf = path.find_last_of("/\\");
	if (lastOf != std::string::npos)
		namePos = lastOf + 1;

	size_t dotPos = path.rfind('.');
	if (dotPos != std::string::npos && dotPos > namePos)
		nameSize = dotPos - namePos;

	return path.substr(namePos, nameSize);
}

std::string Util::getExtension(const std::string& path)
{
	size_t dotPos = path.rfind('.');
	if (dotPos == std::string::npos)
	{
		return "";
	}

	return path.substr(dotPos + 1);
}

std::string Util::toString(int value)
{
	std::string str;

	// Handle zero explicitly
	if (value == 0)
	{
		str = "0";
		return str;
	}

	// Use unsigned to safely handle INT_MIN
	uint32_t uval = static_cast<uint32_t>((value < 0) ? -value : value);

	// Build the string backwards (more efficient than calculating powers of 10)
	while (uval > 0)
	{
		str.push_back('0' + (uval % 10));
		uval /= 10;
	}

	// Add sign and reverse
	if (value < 0)
		str.push_back('-');

	std::reverse(str.begin(), str.end());
	return str;
}
