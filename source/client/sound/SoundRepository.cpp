/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SoundRepository.hpp"
#include "common/Logger.hpp"
#include "common/Mth.hpp"

void SoundRepository::add(const std::string& name, SoundDesc& sd)
{
	if (!sd.m_isLoaded)
		return;
	std::map<std::string, std::vector<SoundDesc> >::iterator iter = m_repo.find(name);
	if (iter == m_repo.end())
	{
		std::vector<SoundDesc> sdv;
		sdv.push_back(sd);
		m_repo.insert(std::make_pair(name, sdv));
	}
	else
	{
		iter->second.push_back(sd);
	}
}

bool SoundRepository::get(const std::string& name, SoundDesc& sd)
{
	// TODO: Who's the genius who decided it'd be better to check a name string rather than an enum?
	std::map<std::string, std::vector<SoundDesc> >::iterator iter = m_repo.find(name);
	if (iter == m_repo.end())
	{
		LOG_E("Couldn't find a sound with id: %s", name.c_str());
		return false;
	}

	int index = Mth::random(int(iter->second.size()));

	sd = iter->second[index];

	return true;
}
