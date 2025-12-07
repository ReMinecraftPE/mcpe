#pragma once

#include "compat/LegacyCPP.hpp"
#include "renderer/hal/enums/BufferType.hpp"

namespace mce
{
    class RenderContext;
	class BufferBase
	{
	public:
        BufferType m_bufferType;
        unsigned int m_stride;
        unsigned int m_count;
        unsigned int m_internalSize;
        unsigned int m_bufferOffset;
		
	protected:
		void _init(BufferBase& other);
		void _move(BufferBase& other);

	public:
        BufferBase();
        ~BufferBase();
        MC_CTOR_MOVE_CUSTOM(BufferBase);

        void releaseBuffer();
        void bindBuffer(RenderContext& context) {}
		void createBuffer(RenderContext& context, unsigned int stride, const void *data, unsigned int count, BufferType bufferType);
		void createDynamicBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType);
        void resizeBuffer(RenderContext& context, const void* data, unsigned int size) {}
        void updateBuffer(RenderContext& context, unsigned int stride, void*& data, unsigned int count);
		void copy(BufferBase& other);
        unsigned int getInternalBufferSize() const { return m_internalSize; }
        bool isValid() const { return false; }

        MC_FUNC_MOVE(BufferBase);
	};
}
