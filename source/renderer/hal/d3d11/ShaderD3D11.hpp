#pragma once

#include <vector>

#include "API_D3D11.hpp"
#include "API_D3D11Compiler.hpp"

#include "ShaderResourceD3D11.hpp"
#include "renderer/VertexFormat.hpp"
#include "renderer/hal/base/ShaderBase.hpp"
#include "renderer/hal/enums/ShaderPrimitiveTypes.hpp"
#include "renderer/hal/interface/ShaderProgram.hpp"
#include "renderer/hal/interface/RenderContext.hpp"

namespace mce
{
    class ShaderD3D11 : public ShaderBase
    {
    private:
        bool m_bCompiledShaders;
        std::vector<ShaderResourceD3D11> m_resourceList;

    public:
        ShaderD3D11(ShaderProgram& vertex, ShaderProgram& fragment, ShaderProgram& geometry);
        ~ShaderD3D11();

    protected:
        ShaderResourceD3D11* _getShaderResource(const std::string& name);

    public:
        void bindVertexPointers(RenderContext& context, const VertexFormat& vertexFormat);
        void bindShader(RenderContext& context, const VertexFormat& format, const void* dataBasePtr, unsigned int shaderStageBits);
        void compileAndLinkShader();
        void reflectShaderResources(ComInterface<ID3D11ShaderReflection> shaderReflection, const D3D11_SHADER_DESC& shaderDesc, ShaderType shaderType);
        void reflectShaderAttributes(ComInterface<ID3D11ShaderReflection> shaderReflection, const D3D11_SHADER_DESC& shaderDesc);
        void reflectShader(const ShaderProgramD3D11& shaderProgram, ShaderType shaderType);
        void reflectBindableResources(ComInterface<ID3D11ShaderReflection> shaderReflection, const D3D11_SHADER_DESC& shaderDesc);
        ComInterface<ID3D11InputLayout> createInputLayout(const VertexFormat& vertexFormat);

        static void SpliceShaderPath(std::string& shaderName);
        static void SpliceShaderExtension(std::string& shaderName);
        static void BuildHeader(std::ostringstream& stream);
    };
}
