#pragma once

#include "API_D3D9.hpp"
#include "renderer/hal/base/BufferBase.hpp"
#include "renderer/hal/enums/MapType.hpp"
#include "thirdparty/com/ComInterface.hpp"

namespace mce
{
    class BufferD3D9 : public BufferBase
    {
    protected:
        ComInterface<IDirect3DVertexBuffer9> m_vertexBuffer;
        ComInterface<IDirect3DIndexBuffer9> m_indexBuffer;
        D3DFORMAT m_format;

    public:
        BufferD3D9();
		MC_CTOR_MOVE(BufferD3D9);
        ~BufferD3D9();

    protected:
        void _createBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType, bool isDynamic);

    public:
		void _move(BufferD3D9& other);
        void releaseBuffer();
        void bindBuffer(RenderContext& context);
		void createBuffer(RenderContext& context, unsigned int stride, const void *data, unsigned int count, BufferType bufferType);
		void createDynamicBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType);
        void resizeBuffer(RenderContext& context, const void* data, unsigned int size);
        void updateBuffer(RenderContext& context, unsigned int stride, void*& data, unsigned int count, MapType mapType = MAP_WRITE_DISCARD);
        bool isValid() const { return m_vertexBuffer || m_indexBuffer; }

        MC_FUNC_MOVE(BufferD3D9);
	};
}