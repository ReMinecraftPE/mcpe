#include "ShaderProgramBase.hpp"

using namespace mce;

ShaderProgramBase::ShaderProgramBase(const std::string& header, const std::string& shaderPath, ShaderType shaderType)
    : m_header(header), m_shaderPath(shaderPath), m_shaderType(shaderType)
{
    m_bValid = false;
}