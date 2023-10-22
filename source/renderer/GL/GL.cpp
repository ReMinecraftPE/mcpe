#include "GL.hpp"

void drawArrayVT(GLuint buffer, int count, int stride)
{
	xglBindBuffer(GL_ARRAY_BUFFER, buffer);
	xglTexCoordPointer(2, GL_FLOAT, stride, (void*)12);
	xglEnableClientState(GL_TEXTURE_COORD_ARRAY);
	xglVertexPointer(3, GL_FLOAT, stride, nullptr);
	xglEnableClientState(GL_VERTEX_ARRAY);
	xglDrawArrays(GL_TRIANGLES, 0, count);
	xglDisableClientState(GL_VERTEX_ARRAY);
	xglDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void drawArrayVTC(GLuint buffer, int count, int stride)
{
	xglBindBuffer(GL_ARRAY_BUFFER, buffer);
	xglVertexPointer(3, GL_FLOAT, stride, nullptr);
	xglTexCoordPointer(2, GL_FLOAT, stride, (void*)12);
	xglColorPointer(4, GL_UNSIGNED_BYTE, stride, (void*)20);
	xglEnableClientState(GL_VERTEX_ARRAY);
	xglEnableClientState(GL_TEXTURE_COORD_ARRAY);
	xglEnableClientState(GL_COLOR_ARRAY);
	xglDrawArrays(GL_TRIANGLES, 0, count);
	xglDisableClientState(GL_VERTEX_ARRAY);
	xglDisableClientState(GL_TEXTURE_COORD_ARRAY);
	xglDisableClientState(GL_COLOR_ARRAY);
}