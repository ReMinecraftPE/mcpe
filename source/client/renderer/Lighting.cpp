#include "Lighting.hpp"

#include "world/phys/Vec3.hpp"

#include "thirdparty/GL/GL.hpp"

float Lighting::lb[4] = {};

void Lighting::turnOff()
{
#ifdef USE_GL_NORMAL_LIGHTING
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_COLOR_MATERIAL);
#endif
}

void Lighting::turnOn()
{
#ifdef USE_GL_NORMAL_LIGHTING
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_COLOR_MATERIAL);
#if !defined(__EMSCRIPTEN__) && !defined(USE_GLES)
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
#endif

	constexpr float a = 0.4f, d = 0.6f, s = 0.0f;

	Vec3 l = Vec3(0.2f, 1.0f, -0.7f).normalize();
	glLightfv(GL_LIGHT0, GL_POSITION, getBuffer(l.x, l.y, l.z, 0.0f));
	glLightfv(GL_LIGHT0, GL_DIFFUSE, getBuffer(d, d, d, 1.0f));
	glLightfv(GL_LIGHT0, GL_AMBIENT, getBuffer(0.0f, 0.0f, 0.0f, 1.0f));
	glLightfv(GL_LIGHT0, GL_SPECULAR, getBuffer(s, s, s, 1.0f));

	l = Vec3(-0.2f, 1.0f, 0.7f).normalize();
	glLightfv(GL_LIGHT1, GL_POSITION, getBuffer(l.x, l.y, l.z, 0.0f));
	glLightfv(GL_LIGHT1, GL_DIFFUSE, getBuffer(d, d, d, 1.0f));
	glLightfv(GL_LIGHT1, GL_AMBIENT, getBuffer(0.0f, 0.0f, 0.0f, 1.0f));
	glLightfv(GL_LIGHT1, GL_SPECULAR, getBuffer(s, s, s, 1.0f));
#endif

	glShadeModel(GL_FLAT);
#ifdef USE_GL_NORMAL_LIGHTING
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(a, a, a, 1.0f));
#endif
}

float *Lighting::getBuffer(float a, float b, float c, float d)
{
	lb[0] = a;
	lb[1] = b;
	lb[2] = c;
	lb[3] = d;
	return (float*)&lb;
}
