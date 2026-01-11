#include "ShaderResourceD3D9.hpp"
#include "ShaderConstantD3D9.hpp"
#include "renderer/hal/interface/ConstantBufferContainer.hpp"

namespace mce
{
    class ShaderUniformD3D9 : public ShaderResourceD3D9
    {
    public:
        ShaderConstantD3D9* m_pShaderConstant;

    public:
        ShaderUniformD3D9();

        void bind(bool forceBind);
    };
}
