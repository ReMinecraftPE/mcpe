#include "ShaderProgramNull.hpp"

using namespace mce;

ShaderProgramNull::ShaderProgramNull(ShaderType shaderType, const std::string& shaderSource, const std::string& header, const std::string& shaderPath)
    : ShaderProgramBase(header, shaderPath, shaderType)
{
}