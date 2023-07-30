/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SoundRepository.hpp"
#include "Utils.hpp"
#include "Mth.hpp"

void SoundRepository::add(const std::string& name, SoundDesc& sd)
{
	auto iter = m_repo.find(name);
	if (iter == m_repo.end())
	{
		std::vector<SoundDesc> sdv;
		sdv.push_back(sd);
		m_repo.insert({ name, sdv });
	}
	else
	{
		iter->second.push_back(sd);
	}
}

bool SoundRepository::get(const std::string& name, SoundDesc& sd)
{
	auto iter = m_repo.find(name);
	if (iter == m_repo.end())
	{
		printf("Couldn't find a sound with id: %s\n", name.c_str());
		return false;
	}

	int index = Mth::random(int(iter->second.size()));

	sd = iter->second[index];

	return true;
}
