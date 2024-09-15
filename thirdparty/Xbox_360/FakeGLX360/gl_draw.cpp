#include <xtl.h>
#include "gl_main.h"
#include "gl_shader_manager.h"

extern int xeGetTmu(GLenum texture);

/***********************************************************************
 * 
 * Batch Rendering
 * 
 **********************************************************************/ 
 
typedef struct gl_varray_pointer_s
{
	GLint size;
	GLenum type;
	GLsizei stride;
	GLvoid *pointer;
} gl_varray_pointer_t;

typedef struct gl_indices_pointer_s
{
	GLenum mode;
	GLenum type;
	GLvoid *pointer;
} gl_indices_pointer_t;

typedef struct gl_vertices_pointer_s
{
	GLint count;
	GLint size;
	GLenum type;
	GLsizei stride;
	GLvoid *pointer;
} gl_vertices_pointer_t;


static gl_indices_pointer_t indexPointer;
static gl_vertices_pointer_t vertexPointer;
static gl_varray_pointer_t colorPointer;
static gl_varray_pointer_t texCoordPointer[XE_MAX_TMUS];
static int vArray_TMU = 0;

/** todo **/
void glClientActiveTexture(GLenum texture)
{
	vArray_TMU = xeGetTmu(texture);
}

void glDrawBuffer (GLenum mode)
{
	// TR
}
 
void glArrayElement(GLint i)
{

}

void glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *	pointer)
{
	if ((type == GL_FLOAT) || (type == GL_UNSIGNED_BYTE)) {	
		// Packed
		if (stride == 0) {
			if ( type == GL_FLOAT )	{
				stride = sizeof(float) * size;
			} else if(type == GL_UNSIGNED_BYTE)	{
				stride = sizeof(char) * size;
			}
		}
		colorPointer.size = size;
		colorPointer.type = type;
		colorPointer.stride = stride;
		colorPointer.pointer = (GLvoid *) pointer;
	} else {
		xe_gl_error("Unimplemented color pointer type");
	}
}

void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	if (type != GL_FLOAT) 
		xe_gl_error("Unimplemented texcoord pointer type\n");

	// Packed
	if (stride == 0) {
		stride = sizeof(float) * size;
	}

	texCoordPointer[vArray_TMU].size = size;
	texCoordPointer[vArray_TMU].type = type;
	texCoordPointer[vArray_TMU].stride = stride;
	texCoordPointer[vArray_TMU].pointer = (GLvoid *) pointer;
}

void glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	if (type != GL_FLOAT) 
		xe_gl_error("Unimplemented vertex pointer type");

	vertexPointer.size = size;
	vertexPointer.type = type;
	vertexPointer.stride = stride;
	vertexPointer.pointer = (GLvoid *) pointer;
}

void glLockArraysEXT(int first, int count) {
	vertexPointer.count = count;
}

void glUnlockArraysEXT(void) {
	// nothing
}


#if 0 // working with vb - slow
/**
 * mode / type
 **/ 
void glDrawElements(GLenum mode, GLsizei numIndexes, GLenum type, const GLvoid * indices)
{
	union {
		float f;
		unsigned int u32;
	} color;
	
	// Begin
	GLImpl.prim.vertices.begin();
	GLImpl.prim.indices.begin();
	
	unsigned int * indexes = (unsigned int*)indices;
	BYTE * vertice_ptr = (BYTE *)vertexPointer.pointer;
	BYTE * color_ptr = (BYTE *)colorPointer.pointer;
	BYTE * texcoords0_ptr = (BYTE *)texCoordPointer[0].pointer;
	BYTE * texcoords1_ptr = (BYTE *)texCoordPointer[1].pointer;

	// vertices
	for (int i = 0 ; i < vertexPointer.count ; i++) {
		float * v = (float*) vertice_ptr;
		float * t0 = (float*) texcoords0_ptr;
		float * t1 = (float*) texcoords1_ptr;
		unsigned char * c = (unsigned char*) color_ptr;		

		color.u32 = D3DCOLOR_ARGB(c[3], c[2], c[1], c[0]);
		//color.u32 = 0xFFFFFFFF;
		
		*GLImpl.prim.vertices.data++ = v[0];
		*GLImpl.prim.vertices.data++ = v[1];
		*GLImpl.prim.vertices.data++ = v[2];
		*GLImpl.prim.vertices.data++ = 1;

		
		if (texcoords0_ptr) {
			*GLImpl.prim.vertices.data++ = t0[0];
			*GLImpl.prim.vertices.data++ = t0[1];
		} else {
			*GLImpl.prim.vertices.data++ = 0;
			*GLImpl.prim.vertices.data++ = 0;
		}
		
		if (texcoords1_ptr) {
			*GLImpl.prim.vertices.data++ = t1[0];
			*GLImpl.prim.vertices.data++ = t1[1];
		} else {
			*GLImpl.prim.vertices.data++ = 0;
			*GLImpl.prim.vertices.data++ = 0;
		}
		
		*GLImpl.prim.vertices.data++ = color.f;		
		
				
		GLImpl.prim.vertices.next();

		vertice_ptr += vertexPointer.stride;
		if (texcoords0_ptr) {
			texcoords0_ptr += 2 * sizeof(float);
		}
		if (texcoords1_ptr) {
			texcoords1_ptr += 2 * sizeof(float);
		}
		color_ptr += 4 * sizeof(char);
	}

	// indices
	for (int i = 0 ; i < numIndexes ; i++) {
		//*GLImpl.prim.indices.data++ = indexes[i] + GLImpl.prim.indices.prev;
		*GLImpl.prim.indices.data++ = indexes[i];
		GLImpl.prim.indices.next();
	}
	
	GLImpl.UpdateStates();
	GLImpl.CheckDirtyMatrix(&GLImpl.projection_matrix);
	GLImpl.CheckDirtyMatrix(&GLImpl.modelview_matrix);
	
	// setup shaders and textures
	shader.ApplyShader();
	GLImpl.ApplyTextures();
	
	GLImpl.device->SetIndices(GLImpl.pIbGL);
	GLImpl.device->SetStreamSource(0, GLImpl.pVbGL, 0, sizeof(glVerticesFormat_t));

	GLImpl.device->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, 
		GLImpl.prim.vertices.prev,
		0,
		0,
		GLImpl.prim.indices.prev, 
		numIndexes/3
	);
}
#endif

#if 1
// fast !!

static float vertices_up[65536];

/**
 * mode / type
 **/ 
void glDrawElements(GLenum mode, GLsizei numIndexes, GLenum type, const GLvoid * indices)
{
	union {
		float f;
		unsigned int u32;
	} color;

	// Begin
	GLImpl.prim.vertices.begin();
	GLImpl.prim.indices.begin();
	
	unsigned int * indexes = (unsigned int*)indices;
	BYTE * vertice_ptr = (BYTE *)vertexPointer.pointer;
	BYTE * color_ptr = (BYTE *)colorPointer.pointer;
	BYTE * texcoords0_ptr = (BYTE *)texCoordPointer[0].pointer;
	BYTE * texcoords1_ptr = (BYTE *)texCoordPointer[1].pointer;
	float * data = &vertices_up[0];

	// vertices
	for (int i = 0 ; i < vertexPointer.count ; i++) {
		float * v = (float*) vertice_ptr;
		float * t0 = (float*) texcoords0_ptr;
		float * t1 = (float*) texcoords1_ptr;
		unsigned char * c = (unsigned char *) color_ptr;	
		color.u32 = D3DCOLOR_ARGB(c[3], c[0], c[1], c[2]);
		
		*data++ = v[0];
		*data++ = v[1];
		*data++ = v[2];
		*data++ = 1;
		
		if (texcoords0_ptr) {
			*data++ = t0[0];
			*data++ = t0[1];
		} else {
			*data++ = 0;
			*data++ = 0;
		}
		
		if (texcoords1_ptr) {
			*data++ = t1[0];
			*data++ = t1[1];
		} else {
			*data++ = 0;
			*data++ = 0;
		}
		
		*data++ = color.f;

		vertice_ptr += vertexPointer.stride;
		if (texcoords0_ptr) {
			texcoords0_ptr += 2 * sizeof(float);
		}
		if (texcoords1_ptr) {
			texcoords1_ptr += 2 * sizeof(float);
		}
		color_ptr += 4 * sizeof(char);
	}

	GLImpl.UpdateStates();
	GLImpl.CheckDirtyMatrix(&GLImpl.projection_matrix);
	GLImpl.CheckDirtyMatrix(&GLImpl.modelview_matrix);
	/*
	GLImpl.device->SetVertexShaderConstantF(16, &GLImpl.state.viewport.MinZ, 1);
	GLImpl.device->SetVertexShaderConstantF(17, &GLImpl.state.viewport.MaxZ, 1);
	*/
	// setup shaders and textures
	shader.ApplyShader();
	GLImpl.ApplyTextures();
//	GLImpl.device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	GLImpl.device->DrawIndexedPrimitiveUP(
		D3DPT_TRIANGLELIST,
		0,
		vertexPointer.count,
		numIndexes/3,
		indexes,
		D3DFMT_INDEX32,
		vertices_up,
		sizeof(glVerticesFormat_t)
	);
}
#endif


#if 0
// not so fast !!

//static float vertices_up[65536];

/**
 * mode / type
 **/ 
void glDrawElements(GLenum mode, GLsizei numIndexes, GLenum type, const GLvoid * indices)
{
	union {
		float f;
		unsigned int u32;
	} color;

	// Begin
	GLImpl.prim.vertices.begin();
	GLImpl.prim.indices.begin();
	
	// unsigned int * indexes = (unsigned int*)indices;
	unsigned int * indexes;
	BYTE * vertice_ptr = (BYTE *)vertexPointer.pointer;
	BYTE * color_ptr = (BYTE *)colorPointer.pointer;
	BYTE * texcoords0_ptr = (BYTE *)texCoordPointer[0].pointer;
	BYTE * texcoords1_ptr = (BYTE *)texCoordPointer[1].pointer;
	float * data;

	

	GLImpl.UpdateStates();
	GLImpl.CheckDirtyMatrix(&GLImpl.projection_matrix);
	GLImpl.CheckDirtyMatrix(&GLImpl.modelview_matrix);
	
	// setup shaders and textures
	shader.ApplyShader();
	GLImpl.ApplyTextures();

	GLImpl.device->BeginIndexedVertices(
		D3DPT_TRIANGLELIST, 0, vertexPointer.count, numIndexes, D3DFMT_INDEX32, sizeof(glVerticesFormat_t), (void**)&indexes, (void**)&data
		);

	memcpy(indexes, indices, numIndexes * 4);

	// vertices
	for (int i = 0 ; i < vertexPointer.count ; i++) {
		float * v = (float*) vertice_ptr;
		float * t0 = (float*) texcoords0_ptr;
		float * t1 = (float*) texcoords1_ptr;
		unsigned char * c = (unsigned char *) color_ptr;	
		color.u32 = D3DCOLOR_ARGB(c[3], c[0], c[1], c[2]);
		
		*data++ = v[0];
		*data++ = v[1];
		*data++ = v[2];
		*data++ = 1;
		
		if (texcoords0_ptr) {
			*data++ = t0[0];
			*data++ = t0[1];
		} else {
			*data++ = 0;
			*data++ = 0;
		}
		
		if (texcoords1_ptr) {
			*data++ = t1[0];
			*data++ = t1[1];
		} else {
			*data++ = 0;
			*data++ = 0;
		}
		
		*data++ = color.f;

		vertice_ptr += vertexPointer.stride;
		if (texcoords0_ptr) {
			texcoords0_ptr += 2 * sizeof(float);
		}
		if (texcoords1_ptr) {
			texcoords1_ptr += 2 * sizeof(float);
		}
		color_ptr += 4 * sizeof(char);
	}

	GLImpl.device->EndIndexedVertices();
	/*
	GLImpl.device->DrawIndexedPrimitiveUP(
		D3DPT_TRIANGLELIST,
		0,
		vertexPointer.count,
		numIndexes/3,
		indexes,
		D3DFMT_INDEX32,
		vertices_up,
		sizeof(glVerticesFormat_t)
	);
	*/
}
#endif

void glDrawArrays (GLenum mode, GLint first, GLsizei count)
{

}

void glEnableClientState(GLenum array)
{
	
}

void glDisableClientState (GLenum array)
{
	// switch the pointer to NULL
	switch (array)
	{
	case GL_VERTEX_ARRAY:
		vertexPointer.pointer = NULL;
		break;

	case GL_COLOR_ARRAY:
		colorPointer.pointer = NULL;
		break;

	case GL_TEXTURE_COORD_ARRAY:
		texCoordPointer[vArray_TMU].pointer = NULL;
		break;

	default:
		xe_gl_error("Invalid Vertex Array Spec...!\n");
	}
}
