/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Util.hpp"

std::string Util::stringTrim(const std::string& str, const std::string& filter, bool a4, bool a5)
{
	if (str.empty() || filter.empty())
		return "";

	// don't know what the hell this does. TODO: clean this crap up
	if (!a4 && !a5)
		return "";

	int v12, strIndex = 0, v13, v17, strLength = int(str.size()), filterLength = int(filter.size());

	if (a4)
	{
		v12 = strLength - 1;
		if (strLength > 0)
		{
			v17 = strLength - 1;
			do
			{
				const void* v14 = memchr(filter.c_str(), str[strIndex], filterLength);
				if (!v14)
				{
					v13 = strIndex;
					v12 = v17;
					goto label_12;
				}

				++strIndex;
			} while (strIndex != strLength);

			v12 = strLength - 1;
			v13 = strIndex;
		}
		else v13 = 0;
	}
	else
	{
		if (a5)
		{
			v12 = strLength - 1;
			strIndex = 0;
			v13 = 0;

			goto label_19;
		}

		return "";
	}

label_12:
	if (a5)
	{
		v13 = strIndex;
	label_19:
		while (v12 >= strIndex)
		{
			const void* v15 = memchr(filter.c_str(), str[strIndex], filterLength);
			if (!v15)
				break;
			--v12;
		}
	}

	return std::string(str, v13, v12 - 1 + strIndex);
}

std::string Util::stringTrim(const std::string& str)
{
	return stringTrim(str, " \t\n\r", true, true);
}

