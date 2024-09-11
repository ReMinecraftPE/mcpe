#pragma once

#include <vector>

class Lighting
{
private:
	static std::vector<float> lb;

public:
	static void turnOff();
	static void turnOn();
	
	static float *getBuffer(float a, float b, float c, float d);
};
