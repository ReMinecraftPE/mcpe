#pragma once

#include <vector>

#include "API_D3D9.hpp"

#include "ShaderResourceD3D9.hpp"
#include "renderer/VertexFormat.hpp"
#include "renderer/hal/base/ShaderBase.hpp"
#include "renderer/hal/enums/ShaderPrimitiveTypes.hpp"
#include "renderer/hal/interface/ShaderProgram.hpp"
#include "renderer/hal/interface/RenderContext.hpp"

// So we don't have to include d3dx9mesh.h, which fails everyhere
struct ID3DXConstantTable;

namespace mce
{
    class ShaderD3D9 : public ShaderBase
    {
    private:
        bool m_bCompiledShaders;
        std::vector<ShaderResourceD3D9> m_resourceList;

    public:
        ShaderD3D9(ShaderProgram& vertex, ShaderProgram& fragment, ShaderProgram& geometry);
        ~ShaderD3D9();

    protected:
        ShaderResourceD3D9* _getShaderResource(const std::string& name);

    public:
        void bindVertexPointers(RenderContext& context, const VertexFormat& vertexFormat);
        void bindShader(RenderContext& context, const VertexFormat& format, const void* dataBasePtr, unsigned int shaderStageBits);
        void compileAndLinkShader();
        void reflectShaderResources(ComInterface<ID3DXConstantTable> constantTable, ShaderType shaderType);
        void reflectShaderAttributes(const std::string& shaderBytecode);
        void reflectShader(const ShaderProgramD3D9& shaderProgram, ShaderType shaderType);
        ComInterface<IDirect3DVertexDeclaration9> createVertexDeclaration(const VertexFormat& vertexFormat);

        static void SpliceShaderPath(std::string& shaderName);
        static void SpliceShaderExtension(std::string& shaderName);
        static void BuildHeader(std::ostringstream& stream);
    };
}
