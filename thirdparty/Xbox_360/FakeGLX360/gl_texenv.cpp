#include "gl_main.h"
#include "gl_shader_manager.h"

/***********************************************************************
 * Texture environnement
 ***********************************************************************/
void glTexEnvf (GLenum target, GLenum pname, GLfloat param)
{
	if (target != GL_TEXTURE_ENV) {
		xe_gl_error("glTexEnvf: unimplemented target\n");
		return;
	}
	
	if (!GLImpl.tmus[GLImpl.current_tmu].boundtexture) return;
	if (!GLImpl.tmus[GLImpl.current_tmu].boundtexture->teximg ) return;

	switch (pname)
	{
		case GL_TEXTURE_ENV_MODE:
			int v;
			switch((GLint)param) {				
				case GL_MODULATE:
					v = TENV_MODULATE;
					break;
				case GL_DECAL:
					v = TENV_DECAL;
					break;
				case GL_BLEND:
					v = TENV_BLEND;
					break;
				case GL_ADD:
					v = TENV_ADD;
					break;
				default:
				case GL_REPLACE:
					v = TENV_REPLACE;
					break;
			}
			shader.SetTexenv(GLImpl.current_tmu, v);
			break;
		default:
			xe_gl_error("glTexEnvf: unimplemented pname\n");
			break;
	}
}

void glTexEnvi (GLenum target, GLenum pname, GLint param)
{
	glTexEnvf(target, pname, (GLfloat)param);
}