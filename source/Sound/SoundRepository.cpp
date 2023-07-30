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
