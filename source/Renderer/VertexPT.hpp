#pragma once

struct VertexPT
{
	float x = 0, y = 0, z = 0;
	float u = 0, v = 0;

	VertexPT() {}
	VertexPT(float x, float y, float z) : x(x), y(y), z(z) {}
	VertexPT(float x, float y, float z, float(u), float(v)) : x(x), y(y), z(z), u(u), v(v) {}

	void setUV(float _u, float _v)
	{
		u = _u, v = _v;
	}
};
