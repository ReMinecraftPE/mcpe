#pragma once

#include "API_D3D11.hpp"

#include "renderer/hal/base/ShaderProgramBase.hpp"
#include "renderer/hal/dxgi/helpers/DirectXComInterface.hpp"

namespace mce
{
    class ShaderProgramD3D11 : public ShaderProgramBase
    {
    public:
        ComInterface<ID3D11VertexShader> m_vertexShader;
        ComInterface<ID3D11PixelShader> m_pixelShader;
        ComInterface<ID3D11GeometryShader> m_geometryShader;
        std::string m_shaderBytecode;
        std::string m_shaderSource;

    public:
        ShaderProgramD3D11(ShaderType shaderType, std::string& shaderSource, const std::string& header, const std::string& shaderPath);
        ~ShaderProgramD3D11();

    public:
        void compileShaderProgram();
    };
}
