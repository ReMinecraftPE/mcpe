#pragma once

#include "renderer/hal/interface/FogState.hpp"

class Fog
{
private:
	static mce::FogState _currentState;
public:
	static mce::FogStateDescription nextState;

private:
	static void _updateState(const mce::FogStateDescription desc);
	static void _setEnabled(bool enabled);
public:
	static void updateState();
	static void updateRange(float startZ, float endZ);
	static void enable();
	static void disable();
};
