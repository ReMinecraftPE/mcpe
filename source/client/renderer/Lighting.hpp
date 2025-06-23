#pragma once

#include <vector>

class Lighting
{
private:
	static float lb[4];

public:
	static void turnOff();
	static void turnOn();
	
	static float *getBuffer(float a, float b, float c, float d);
};
