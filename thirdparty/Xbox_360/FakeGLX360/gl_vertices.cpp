#include "gl_xenos.h"
#include "gl_shader_manager.h"

static D3DPRIMITIVETYPE d3dprim[] = {
	D3DPT_POINTLIST,// GL_POINTS 0x0000
	D3DPT_LINELIST,// GL_LINES 0x0001
	D3DPT_LINELIST,// GL_LINE_LOOP 0x0002
	D3DPT_LINESTRIP,// GL_LINE_STRIP 0x0003
	D3DPT_TRIANGLELIST,// GL_TRIANGLES 0x0004
	D3DPT_TRIANGLESTRIP,// GL_TRIANGLE_STRIP 0x0005
	D3DPT_TRIANGLEFAN,// GL_TRIANGLE_FAN 0x0006
	D3DPT_QUADLIST,// GL_QUADS 0x0007
	D3DPT_QUADLIST,// GL_QUAD_STRIP 0x0008
	D3DPT_TRIANGLEFAN// GL_POLYGON 0x0009
};

static int primSize(D3DPRIMITIVETYPE prim, int size) {
	return (size / D3DPRIMITIVEVERTEXCOUNT[prim][0]) - D3DPRIMITIVEVERTEXCOUNT[prim][1];
}

#if !USE_VB
static float * verts = NULL;
//static float vertices_up[65536];
static float vertices_up[131072]; // BDC
static int vertices_count;
#endif

void CGLImpl::SubmitVertexes()
{	
	// only render if we have some vertices
	if (prim.vertices.size() == 0 && vertices_count == 0)
		return;

	//device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		
	// update states if dirty
	UpdateStates();

	// update if dirty
	CheckDirtyMatrix(&projection_matrix);
	CheckDirtyMatrix(&modelview_matrix);

	/*
	GLImpl.device->SetVertexShaderConstantF(16, &GLImpl.state.viewport.MinZ, 1);
	GLImpl.device->SetVertexShaderConstantF(17, &GLImpl.state.viewport.MaxZ, 1);
	*/
	ApplyTextures();
    shader.ApplyShader();

	device->DrawPrimitiveUP(d3dprim[prim.primitive_type], primSize(d3dprim[prim.primitive_type], (vertices_count)), vertices_up, sizeof(glVerticesFormat_t));
}


void glBegin(GLenum mode)
{
#if !USE_VB
	vertices_count = 0;
	verts = &vertices_up[0];
#endif
	GLImpl.prim.primitive_type = mode;
	
	GLImpl.prim.vertices.begin();
	GLImpl.prim.indices.begin();
}

void glEnd()
{
	// submit vertices
	GLImpl.SubmitVertexes();
	GLImpl.prim.color.u32 = 0;
};



void glVertex3f (GLfloat x, GLfloat y, GLfloat z)
{	
#if USE_VB
	*GLImpl.prim.vertices.data++ = x;
	*GLImpl.prim.vertices.data++ = y;
	*GLImpl.prim.vertices.data++ = z;
	*GLImpl.prim.vertices.data++ = 1;

	for(int i = 0; i < XE_MAX_TMUS; i++) {
		*GLImpl.prim.vertices.data++ = GLImpl.prim.uv[i].u;
		*GLImpl.prim.vertices.data++ = GLImpl.prim.uv[i].v;
	}
	*GLImpl.prim.vertices.data++ = GLImpl.prim.color.f;

	// next vertex
	GLImpl.prim.vertices.next();
#else

	*verts++ = x;
	*verts++ = y;
	*verts++ = z;
	*verts++ = 1;

	for(int i = 0; i < XE_MAX_TMUS; i++) {
		*verts++ = GLImpl.prim.uv[i].u;
		*verts++ = GLImpl.prim.uv[i].v;
	}
	*verts++ = GLImpl.prim.color.f;

	vertices_count++;
#endif
}

void glVertex2fv (const GLfloat *v)
{
	glVertex3f (v[0], v[1], 0);
}


void glVertex2f (GLfloat x, GLfloat y)
{
	glVertex3f (x, y, 0);
}


void glVertex3fv (const GLfloat *v)
{
	glVertex3f (v[0], v[1], v[2]);
}

void glTexCoord2f (GLfloat s, GLfloat t)
{
	GLImpl.prim.uv[0].u = s;
	GLImpl.prim.uv[0].v = t;
}


void glTexCoord2fv (const GLfloat *v)
{
	GLImpl.prim.uv[0].u = v[0];
	GLImpl.prim.uv[0].v = v[1];
}


void glFinish (void)
{
	
}

GLenum glGetError(){
	return GL_NO_ERROR;
}

void glPointSize(GLfloat size)
{
	
}
void glPointParameterf(	GLenum pname, GLfloat param)
{
	
}

void glPointParameterfv(GLenum pname,const GLfloat *  params)
{
	
}
