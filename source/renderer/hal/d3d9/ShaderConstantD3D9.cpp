#include "ShaderConstantD3D9.hpp"

using namespace mce;

void ShaderConstantD3D9::syncConstant(RenderContext& context, ShaderType shaderType, unsigned int location)
{
    m_dirty = false;
}