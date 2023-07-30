#pragma once

#include <vector>

class Controller
{
public:
	static bool isValidStick(int stickNo);
	static float linearTransform(float, float, float, bool);
	static void feed(int stickNo, int touched, float x, float y);
	static float getX(int stickNo);
	static float getY(int stickNo);
	static float getTransformedX(int stickNo, float a2, float a3, bool b);
	static float getTransformedY(int stickNo, float a2, float a3, bool b);
	static bool isTouched(int stickNo);

public:
	static bool isTouchedValues[2];
	static float stickValuesX[2], stickValuesY[2];
};

