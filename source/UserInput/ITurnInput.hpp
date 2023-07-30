#pragma once

class ITurnInput
{
public:
	struct Delta
	{
		float x = 0.0f, y = 0.0f;
		Delta() {}
		Delta(float x, float y) : x(x), y(y) {}
	};

public:
	float getDeltaTime();
	virtual ~ITurnInput();
	virtual Delta getTurnDelta() = 0;

private:
	float m_prevTime = -1.0f;
};

