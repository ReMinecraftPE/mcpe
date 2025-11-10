#include "ShaderProgram.hpp"

using namespace mce;

ShaderProgram::ShaderProgram(ShaderType shaderType, const std::string& shaderSource, const std::string& header, const std::string& shaderPath)
    : MCE_GFX_CLASS(ShaderProgram)(shaderType, shaderSource, header, shaderPath)
{
}