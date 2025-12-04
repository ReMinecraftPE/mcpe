#pragma once

class Matrix;
class Lighting
{
public:
	static void turnOff(bool teardown = true);
	static void turnOn(bool init = true);
	static void turnOn(const Matrix& matrix);
};
