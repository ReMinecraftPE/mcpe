#pragma once

#include "renderer/hal/base/ConstantBufferContainerBase.hpp"

namespace mce
{
    class ConstantBufferContainerD3D9 : public ConstantBufferContainerBase
    {
    public:
#if MCE_GFX_D3D9_SHADER_CONSTANT_BUFFERS
        ComInterface<IDirect3DConstantBuffer9> m_buffer;
		unsigned int m_vector4fCount;
#endif

    public:
        ConstantBufferContainerD3D9();

	protected:
		unsigned int _getVector4fCount() const;

    public:
        void sync(RenderContext& context);
        void allocateRenderContextBuffer(RenderContext& context);
        void bindBuffer(RenderContext& context, unsigned int shaderBindPoint, unsigned int shaderStagesBits);
        void* lock(RenderContext& context);
        void unlock(RenderContext& context);
    };
}