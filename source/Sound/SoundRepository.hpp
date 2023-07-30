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
	std::map<std::string, std::vector<SoundDesc>> m_repo;
};

