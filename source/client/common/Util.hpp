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
#include "LongHack.hpp"

class Util
{
public:
	static std::string stringTrim(const std::string &, const std::string &, bool, bool);
	static std::string stringTrim(const std::string &);

	template<typename T>
	static bool remove(std::vector<T>& vec, const T& t)
	{
		auto iter = std::find(vec.begin(), vec.end(), t);
		if (iter == vec.end())
			return false;

		vec.erase(iter);
		return true;
	}

	template<typename T>
	static int removeAll(std::vector<T>& vec, const std::vector<T>& toRemove)
	{
		int removed = 0;

		for (auto rem : toRemove)
		{
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

	static TLong hashCode(const std::string& str)
	{
		TLong result = 0;

		for (auto chr : str)
		{
			result = result * 31 + chr;
		}

		return result;
	}
};
