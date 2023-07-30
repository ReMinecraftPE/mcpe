#include "MemoryLevelStorage.hpp"
#include "MemoryLevelStorageSource.hpp"

std::string MemoryLevelStorageSource::getName()
{
	return "Memory Storage";
}

LevelStorage* MemoryLevelStorageSource::selectLevel(const std::string& x, bool y)
{
	return new MemoryLevelStorage;
}

void MemoryLevelStorageSource::clearAll()
{
}

int MemoryLevelStorageSource::getDataTagFor(const std::string& x)
{
	return 0;
}

bool MemoryLevelStorageSource::isNewLevelIdAcceptable(const std::string& x)
{
	return true;
}

void MemoryLevelStorageSource::deleteLevel(const std::string& x)
{
	LogMsg("Delete level: %s", x.c_str());
}

void MemoryLevelStorageSource::renameLevel(const std::string& x, const std::string& y)
{
}

bool MemoryLevelStorageSource::isConvertible(const std::string& x)
{
	return false;
}

bool MemoryLevelStorageSource::requiresConversion(const std::string& x)
{
	return false;
}

void MemoryLevelStorageSource::convertLevel(const std::string& x, ProgressListener* y)
{

}
