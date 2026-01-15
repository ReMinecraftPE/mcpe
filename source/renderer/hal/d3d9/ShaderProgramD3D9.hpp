#pragma once

#include "API_D3D9.hpp"

#include "renderer/hal/base/ShaderProgramBase.hpp"
#include "renderer/hal/dxgi/helpers/DirectXComInterface.hpp"

namespace mce
{
    class ShaderProgramD3D9 : public ShaderProgramBase
    {
    public:
        ComInterface<IDirect3DVertexShader9> m_vertexShader;
        ComInterface<IDirect3DPixelShader9> m_pixelShader;
        std::string m_shaderBytecode;
        std::string m_shaderSource;

    public:
        ShaderProgramD3D9(ShaderType shaderType, std::string& shaderSource, const std::string& header, const std::string& shaderPath);
        ~ShaderProgramD3D9();

    public:
        void compileShaderProgram();
    };
}
