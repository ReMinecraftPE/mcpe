/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <cstring>
#include <algorithm>
#include <string>
#include <vector>
#include <stdint.h>
#include <stdarg.h>

class Util
{
public:
	static const std::string EMPTY_STRING;

public:
	static std::string stringTrim(const std::string &, const std::string &, bool, bool);
	static std::string stringTrim(const std::string &);

	static std::string vformat(const char* fmt, va_list argPtr);
	static std::string format(const char* fmt, ...);

	template<typename T>
	static bool remove(std::vector<T>& vec, const T& t)
	{
		typename std::vector<T>::iterator iter = std::find(vec.begin(), vec.end(), t);
		if (iter == vec.end())
			return false;

		vec.erase(iter);
		return true;
	}

	template<typename T>
	static int removeAll(std::vector<T>& vec, const std::vector<T>& toRemove)
	{
		int removed = 0;

		for (typename std::vector<T>::const_iterator it = toRemove.begin(); it != toRemove.end(); it++)
		{
			T rem = *it;

			if (remove(vec, rem))
				removed++;
		}

		return removed;
	}

	// @TODO: reverse the actual thing? This is something different, but I'm lazy. It uses std::string::replace
	static void stringReplace(std::string& in, const std::string& what, const std::string& with)
	{
		//snippet from Zahlman's post on gamedev:  http://www.gamedev.net/community/forums/topic.asp?topic_id=372125
		size_t pos = 0;
		size_t whatLen = what.length();
		size_t withLen = with.length();
		while ((pos = in.find(what, pos)) != std::string::npos)
		{
			in.replace(pos, whatLen, with);
			pos += withLen;
		}
	}

	static int32_t hashCode(const std::string& str)
	{
		int32_t result = 0;

		const size_t size = str.size();
		for (int i = 0; i < size; i++)
		{
			result = result * 31 + str.at(i);
		}


		return result;
	}
};
