#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gl_xenos.h"

// point to current matrix
static xe_matrix_t * current_matrix = NULL;
 
#define CURRENT_MATRIX_STACK current_matrix->stack[current_matrix->stackdepth]
 
void glMatrixMode (GLenum mode) {
	switch (mode)
	{
		case GL_MODELVIEW:
			current_matrix = &GLImpl.modelview_matrix;
			break;

		case GL_PROJECTION:
			current_matrix = &GLImpl.projection_matrix;
			break;

		default:
			xe_gl_error ("glMatrixMode: unimplemented mode\n");
			break;
	}
}

void glLoadIdentity(void) {
	CURRENT_MATRIX_STACK = XMMatrixIdentity();
	current_matrix->dirty = 1;
}


void glLoadMatrixf (const GLfloat *m) {
	memcpy (CURRENT_MATRIX_STACK.m, m, sizeof(XMMATRIX));
	current_matrix->dirty = 1;
}


void glFrustum (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar) {
	XMMATRIX tmp = XMMatrixPerspectiveOffCenterRH(left, right, bottom, top, zNear, zFar);
	CURRENT_MATRIX_STACK = tmp * CURRENT_MATRIX_STACK;

	current_matrix->dirty = 1;
}

void glOrtho (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar) {
	XMMATRIX tmp = XMMatrixOrthographicOffCenterRH(left, right, bottom, top, zNear, zFar);
	CURRENT_MATRIX_STACK = tmp * CURRENT_MATRIX_STACK;

	current_matrix->dirty = 1;
}


void glPopMatrix (void) {
	if (!current_matrix->stackdepth)
	{
		// opengl silently allows this and so should we (witness TQ's R_DrawAliasModel, which pushes the
		// matrix once but pops it twice - on line 423 and line 468
		current_matrix->dirty = 1;
		return;
	}

	// go to a new matrix
	current_matrix->stackdepth--;

	// flag as dirty
	current_matrix->dirty = 1;
}


void glPushMatrix (void) {
	if (current_matrix->stackdepth <= (MAX_MATRIX_STACK - 1))
	{
		// go to a new matrix (only push if there's room to push)
		current_matrix->stackdepth++;

		// copy up the previous matrix (per spec)
		memcpy
		(
			&current_matrix->stack[current_matrix->stackdepth],
			&current_matrix->stack[current_matrix->stackdepth - 1],
			sizeof(XMMATRIX)
		);
	}

	// flag as dirty
	current_matrix->dirty = 1;
}


void glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
	// replicates the OpenGL glRotatef with 3 components and angle in degrees
	XMVECTOR vec;
	XMMATRIX mat;

	vec.x = x;
	vec.y = y;
	vec.z = z;

	mat = XMMatrixRotationAxis(vec, D3DXToRadian(angle));
	current_matrix->stack[current_matrix->stackdepth] = mat * current_matrix->stack[current_matrix->stackdepth];

	// dirty the matrix
	current_matrix->dirty = 1;
}


void glScalef (GLfloat x, GLfloat y, GLfloat z) {
	XMMATRIX mat = XMMatrixScaling(x, y, z);
	current_matrix->stack[current_matrix->stackdepth] = mat * current_matrix->stack[current_matrix->stackdepth];

	// dirty the matrix
	current_matrix->dirty = 1;
}


void glTranslatef (GLfloat x, GLfloat y, GLfloat z) {
	XMMATRIX mat = XMMatrixTranslation(x, y, z);
	current_matrix->stack[current_matrix->stackdepth] = mat * current_matrix->stack[current_matrix->stackdepth];

	// dirty the matrix
	current_matrix->dirty = 1;
}

void glGetFloatv (GLenum pname, GLfloat *params) {
	switch (pname)
	{
	case GL_MODELVIEW_MATRIX:
		memcpy (params, &GLImpl.modelview_matrix.stack[current_matrix->stackdepth].m, sizeof(XMMATRIX));
		break;
	case GL_PROJECTION_MATRIX:
		memcpy (params, &GLImpl.projection_matrix.stack[current_matrix->stackdepth].m, sizeof(XMMATRIX));
		break;
	default:
		break;
	}
}

void glMultMatrixf (const GLfloat *m) {
	XMMATRIX mat;

	memcpy(mat.m, m, sizeof(XMMATRIX));
	current_matrix->stack[current_matrix->stackdepth] = mat * current_matrix->stack[current_matrix->stackdepth];

	// dirty the matrix
	current_matrix->dirty = 1;
}

void CGLImpl::InitializeMatrix(xe_matrix_t *m) {
	// initializes a matrix to a known state prior to rendering
	m->dirty = 1;
	m->stackdepth = 0;
	m->stack[0] = XMMatrixIdentity();
}


void CGLImpl::CheckDirtyMatrix(xe_matrix_t *m) {
	if (m->dirty)
	{
		/** Hack **/
		if(m->usage == MATPROJECTION) {
			XMMATRIX mat = XMMatrixMultiply(m->stack[m->stackdepth], depth_fix);
			device->SetVertexShaderConstantF(m->usage, (float*)&mat, 4);
		} else {
			device->SetVertexShaderConstantF(m->usage, (float*)&m->stack[m->stackdepth], 4);
		}
		m->dirty = 0;
	}
}

void CGLImpl::ResetMatrixDirty() {
	modelview_matrix.dirty = 1;
	projection_matrix.dirty = 1;
}

void CGLImpl::InitializeMatrices() {
	projection_matrix.dirty = 1;
	projection_matrix.stackdepth = 0;
	projection_matrix.usage = MATPROJECTION;

	modelview_matrix.dirty = 1;
	modelview_matrix.stackdepth = 0;
	modelview_matrix.usage = MATMODELVIEW;

	depth_fix = XMMatrixMultiply(XMMatrixTranslation(0, 0, 1), XMMatrixScaling(1, 1, 0.5));
}
