#include "ShaderConstantWithDataOGL.hpp"

#if FEATURE_GFX_SHADERS

using namespace mce;

template <>
void ShaderConstantWithDataOGL<SHADER_PRIMITIVE_FLOAT1>::syncUniform(int value)    { xglUniform1fv(value, 1, (const GLfloat*)m_data); }
template <>
void ShaderConstantWithDataOGL<SHADER_PRIMITIVE_FLOAT2>::syncUniform(int value)    { xglUniform2fv(value, 1, (const GLfloat*)m_data); }
template <>
void ShaderConstantWithDataOGL<SHADER_PRIMITIVE_FLOAT3>::syncUniform(int value)    { xglUniform3fv(value, 1, (const GLfloat*)m_data); }
template <>
void ShaderConstantWithDataOGL<SHADER_PRIMITIVE_FLOAT4>::syncUniform(int value)    { xglUniform4fv(value, 1, (const GLfloat*)m_data); }
template <>
void ShaderConstantWithDataOGL<SHADER_PRIMITIVE_INT1>::syncUniform(int value)      { xglUniform1iv(value, 1, (const GLint*)m_data); }
template <>
void ShaderConstantWithDataOGL<SHADER_PRIMITIVE_INT2>::syncUniform(int value)      { xglUniform2iv(value, 1, (const GLint*)m_data); }
template <>
void ShaderConstantWithDataOGL<SHADER_PRIMITIVE_INT3>::syncUniform(int value)      { xglUniform3iv(value, 1, (const GLint*)m_data); }
template <>
void ShaderConstantWithDataOGL<SHADER_PRIMITIVE_INT4>::syncUniform(int value)      { xglUniform4iv(value, 1, (const GLint*)m_data); }
template <>
void ShaderConstantWithDataOGL<SHADER_PRIMITIVE_MATRIX2x2>::syncUniform(int value) { xglUniformMatrix2fv(value, 1, 0, (const GLfloat*)m_data); }
template <>
void ShaderConstantWithDataOGL<SHADER_PRIMITIVE_MATRIX3x3>::syncUniform(int value) { xglUniformMatrix3fv(value, 1, 0, (const GLfloat*)m_data); }
template <>
void ShaderConstantWithDataOGL<SHADER_PRIMITIVE_MATRIX4x4>::syncUniform(int value) { xglUniformMatrix4fv(value, 1, 0, (const GLfloat*)m_data); }

#endif // FEATURE_GFX_SHADERS