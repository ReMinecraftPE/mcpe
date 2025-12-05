#include "FogStateDescription.hpp"

using namespace mce;

FogStateDescription::FogStateDescription()
{
	enableFog = false;
	fogDensity = 1.0f;
	fogStartZ = 0.0f;
	fogEndZ = 0.0f;
	fogMode = FOG_MODE_EXP;
}

bool FogStateDescription::operator==(const FogStateDescription& other) const
{
	return enableFog == other.enableFog
		&& fogMode == other.fogMode
		&& fogDensity == other.fogDensity
		&& fogStartZ == other.fogStartZ
		&& fogEndZ == other.fogEndZ
		&& fogColor == other.fogColor;
}

bool FogStateDescription::operator!=(const FogStateDescription& other) const
{
	return !(*this == other);
}
