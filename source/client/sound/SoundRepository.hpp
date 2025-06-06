/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include <vector>
#include <map>
#include "SoundData.hpp"

class SoundRepository
{
public:
	void add(const std::string& name, SoundDesc& sd);
	bool get(const std::string& name, SoundDesc& sd);

public:
	std::map<std::string, std::vector<SoundDesc> > m_repo;
};
