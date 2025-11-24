#pragma once

#include <vector>

#include "API_OGL.hpp"

#if FEATURE_GFX_SHADERS

#include "ShaderUniformOGL.hpp"
#include "renderer/VertexFormat.hpp"
#include "renderer/hal/base/ShaderBase.hpp"
#include "renderer/hal/enums/ShaderPrimitiveTypes.hpp"
#include "renderer/hal/helpers/ErrorHandler.hpp"
#include "renderer/hal/interface/ShaderProgram.hpp"
#include "renderer/hal/interface/RenderContext.hpp"

namespace mce
{
    class ShaderOGL : public ShaderBase
    {
    public:
        struct VertexFieldFormat
        {
            GLenum componentsType;
            GLint components;
            GLboolean normalized;
        };

    private:
        GLuint m_program;
        std::vector<ShaderUniformOGL> m_uniformList;
        std::vector<ShaderResourceOGL> m_textureList;

    public:
        ShaderOGL(ShaderProgram& vertex, ShaderProgram& fragment, ShaderProgram& geometry);
        ~ShaderOGL();

    public:
        static ShaderPrimitiveTypes shaderPrimitiveTypeFromOGLUniformType(GLenum uniformType);

        void deleteShader();
        void finalizeShaderUniforms();
        static void freeCompilerResources();
        static void resetLastProgram();
        void createAndAttachPrograms();
        void linkShader();
        void bindVertexPointers(const VertexFormat&, const void*);
        void bindShader(RenderContext& context, const VertexFormat& format, const void *dataBasePtr, unsigned int shaderStageBits);
        void reflectShaderUniforms();
        void reflectShaderAttributes();
        void reflectShader();
    };
}

#endif // FEATURE_GFX_SHADERS