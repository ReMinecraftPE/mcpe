/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ExternalFileLevelStorageSource.hpp"
#include "ExternalFileLevelStorage.hpp"
#include "client/common/Util.hpp"

#ifndef DEMO

ExternalFileLevelStorageSource::ExternalFileLevelStorageSource(const std::string& path)
{
	m_worldsPath = path;

	m_worldsPath += "/games";
	if (createFolderIfNotExists(m_worldsPath.c_str()))
	{
		m_worldsPath += "/com.mojang";
		if (createFolderIfNotExists(m_worldsPath.c_str()))
		{
			m_worldsPath += "/minecraftWorlds";
			if (createFolderIfNotExists(m_worldsPath.c_str()))
			{
				std::vector<LevelSummary> vls;
				getLevelList(vls);
			}
		}
	}

	m_worldsPath = path + "/games" + "/com.mojang" + "/minecraftWorlds";
}

std::string ExternalFileLevelStorageSource::getName()
{
	return "External File Level Storage";
}

LevelStorage* ExternalFileLevelStorageSource::selectLevel(const std::string& name, bool b)
{
	return new ExternalFileLevelStorage(name, m_worldsPath + "/" + name);
}

void ExternalFileLevelStorageSource::getLevelList(std::vector<LevelSummary>& vls)
{
	DIR* dir = opendir(m_worldsPath.c_str());
	if (!dir)
		return;

	while (true)
	{
		dirent* de = readdir(dir);
		if (!de)
			break;

		LogMsg("Entry: %s", de->d_name);

		if (de->d_type == DT_DIR)
		{
			addLevelSummaryIfExists(vls, de->d_name);
		}
	}

	closedir(dir);
}

void ExternalFileLevelStorageSource::clearAll()
{
}

int ExternalFileLevelStorageSource::getDataTagFor(const std::string& str)
{
	return 0;
}

bool ExternalFileLevelStorageSource::isNewLevelIdAcceptable(const std::string& levelID)
{
	return true;
}

static char g_EFLSSFilterArray[] = { '/','\n','\r','\x09','\0','\xC','`','?','*','\\','<','>','|','"',':' };

void ExternalFileLevelStorageSource::deleteLevel(const std::string& levelName)
{
	std::stringstream ss;
	ss << m_worldsPath << "/" << levelName;
	std::string path = ss.str();

	if (DeleteDirectory(path, true))
		return;

	remove((path + "/chunks.dat").c_str());
	remove((path + "/player.dat").c_str());
	remove((path + "/level.dat").c_str());
}

void ExternalFileLevelStorageSource::renameLevel(const std::string& oldName, const std::string& newName)
{
	int accessResult = XPL_ACCESS((m_worldsPath + "/" + oldName).c_str(), 0);
	if (accessResult)
		return;

	std::string levelName = Util::stringTrim(newName);
	for (int i = 0; i < sizeof(g_EFLSSFilterArray); i++)
	{
		std::string str;
		str.push_back(g_EFLSSFilterArray[i]);
		Util::stringReplace(levelName, str, "");
	}

	std::vector<LevelSummary> vls;
	getLevelList(vls);

	std::set<std::string> maps;

	for (const auto& ls : vls)
		maps.insert(ls.field_0);

	std::string levelUniqueName = levelName;
	while (maps.find(levelUniqueName) != maps.end())
		levelUniqueName += "-";

	int renameResult = rename((m_worldsPath + "/" + oldName).c_str(), (m_worldsPath + "/" + levelUniqueName).c_str());
	if (renameResult != 0)
		levelUniqueName = oldName;

	LevelData ld;
	ExternalFileLevelStorage::readLevelData(m_worldsPath + "/" + levelUniqueName + "/" + "level.dat", &ld);
	ld.setLevelName(levelName);
	ExternalFileLevelStorage::writeLevelData(m_worldsPath + "/" + levelUniqueName + "/" + "level.dat", &ld);
}

bool ExternalFileLevelStorageSource::isConvertible(const std::string&)
{
	return false;
}

bool ExternalFileLevelStorageSource::requiresConversion(const std::string&)
{
	return false;
}

int ExternalFileLevelStorageSource::convertLevel(const std::string&, ProgressListener*)
{
	return 0;
}

void ExternalFileLevelStorageSource::addLevelSummaryIfExists(std::vector<LevelSummary>& vls, const char* name)
{
	std::string levelDat = m_worldsPath + "/" + name + "/" + "level.dat";
	
	LevelData ld;
	
	if (!ExternalFileLevelStorage::readLevelData(levelDat, &ld))
		return;

	LevelSummary ls;
	ls.field_0 = name;
	ls.field_18 = ld.field_78;
	ls.field_30 = ld.field_14;
	ls.field_34 = ld.field_18;
	vls.push_back(ls);
}

#endif
