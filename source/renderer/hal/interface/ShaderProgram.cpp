#include "ShaderProgram.hpp"

using namespace mce;

ShaderProgram::ShaderProgram(ShaderType shaderType, const std::string& shaderSource, const std::string& header, const std::string& shaderPath)
    : ShaderProgramOGL(shaderType, shaderSource, header, shaderPath)
{
}