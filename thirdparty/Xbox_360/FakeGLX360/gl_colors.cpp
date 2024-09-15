#include <xtl.h>
#include "gl_xenos.h"

#define BYTE_CLAMP(i) (int) ((((i) > 255) ? 255 : (((i) < 0) ? 0 : (i))))

void GL_SetColor (float red, float green, float blue, float alpha)
{
	// overwrite color incase verts set it
	GLImpl.prim.color.u32 = D3DCOLOR_ARGB(BYTE_CLAMP(alpha), BYTE_CLAMP(red), BYTE_CLAMP(green), BYTE_CLAMP(blue));
}

void glColor3f (GLfloat red, GLfloat green, GLfloat blue)
{
	GL_SetColor (red * 255, green * 255, blue * 255, 255);
}


void glColor3fv (const GLfloat *v)
{
	GL_SetColor (v[0] * 255, v[1] * 255, v[2] * 255, 255);
}


void glColor3ubv (const GLubyte *v)
{
	GLImpl.prim.color.u32 = D3DCOLOR_ARGB(255, v[0], v[1], v[2]);
}


void glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	GL_SetColor (red * 255, green * 255, blue * 255, alpha * 255);
}


void glColor4fv (const GLfloat *v)
{
	GL_SetColor (v[0] * 255, v[1] * 255, v[2] * 255, v[3] * 255);
}

void glColor4ubv (const GLubyte *v)
{
	GLImpl.prim.color.u32 = D3DCOLOR_ARGB(v[3], v[0], v[1], v[2]);
}


void glColor4ub (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
	GL_SetColor (red, green, blue, alpha);
}

