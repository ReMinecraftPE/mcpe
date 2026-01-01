#include "ShaderResourceOGL.hpp"
#include "ShaderConstantOGL.hpp"
#include "renderer/hal/interface/ConstantBufferContainer.hpp"

namespace mce
{
    // ShaderUniform is specific to OpenGL
    class ShaderUniformOGL : public ShaderResourceOGL
    {
    public:
        ShaderConstantOGL* m_pShaderConstant;
        ConstantBufferContainer* m_pConstantBufferContainer;

    public:
        ShaderUniformOGL(const std::string& name, int location, int elements, ShaderPrimitiveTypes shaderPrimitiveType);

        void bind(bool forceBind);
    };
}
