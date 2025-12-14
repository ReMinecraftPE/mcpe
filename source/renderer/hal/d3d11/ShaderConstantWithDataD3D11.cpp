#include "ShaderConstantWithDataOGL.hpp"

#ifdef FEATURE_GFX_SHADERS

using namespace mce;

template <>
void ShaderConstantWithDataOGL<SHADER_PRIMITIVE_FLOAT1>::syncUniform(int location)    { xglUniform1fv(location, 1, (const GLfloat*)m_data); }
template <>
void ShaderConstantWithDataOGL<SHADER_PRIMITIVE_FLOAT2>::syncUniform(int location)    { xglUniform2fv(location, 1, (const GLfloat*)m_data); }
template <>
void ShaderConstantWithDataOGL<SHADER_PRIMITIVE_FLOAT3>::syncUniform(int location)    { xglUniform3fv(location, 1, (const GLfloat*)m_data); }
template <>
void ShaderConstantWithDataOGL<SHADER_PRIMITIVE_FLOAT4>::syncUniform(int location)    { xglUniform4fv(location, 1, (const GLfloat*)m_data); }
template <>
void ShaderConstantWithDataOGL<SHADER_PRIMITIVE_INT1>::syncUniform(int location)      { xglUniform1iv(location, 1, (const GLint*)m_data); }
template <>
void ShaderConstantWithDataOGL<SHADER_PRIMITIVE_INT2>::syncUniform(int location)      { xglUniform2iv(location, 1, (const GLint*)m_data); }
template <>
void ShaderConstantWithDataOGL<SHADER_PRIMITIVE_INT3>::syncUniform(int location)      { xglUniform3iv(location, 1, (const GLint*)m_data); }
template <>
void ShaderConstantWithDataOGL<SHADER_PRIMITIVE_INT4>::syncUniform(int location)      { xglUniform4iv(location, 1, (const GLint*)m_data); }
template <>
void ShaderConstantWithDataOGL<SHADER_PRIMITIVE_MATRIX2x2>::syncUniform(int location) { xglUniformMatrix2fv(location, 1, 0, (const GLfloat*)m_data); }
template <>
void ShaderConstantWithDataOGL<SHADER_PRIMITIVE_MATRIX3x3>::syncUniform(int location) { xglUniformMatrix3fv(location, 1, 0, (const GLfloat*)m_data); }
template <>
void ShaderConstantWithDataOGL<SHADER_PRIMITIVE_MATRIX4x4>::syncUniform(int location) { xglUniformMatrix4fv(location, 1, 0, (const GLfloat*)m_data); }

#endif // FEATURE_GFX_SHADERS