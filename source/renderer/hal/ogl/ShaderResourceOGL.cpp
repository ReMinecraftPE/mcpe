#include "ShaderResourceOGL.hpp"

using namespace mce;

ShaderResourceOGL::ShaderResourceOGL(const std::string& name, int location, int elements, ShaderPrimitiveTypes shaderPrimitiveType)
    : m_location(location)
    , m_elements(elements)
    , m_shaderPrimitiveType(shaderPrimitiveType)
    , m_name(name)
{
    m_bValid = false;
}