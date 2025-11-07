#pragma once

#include "renderer/hal/enums/BufferType.hpp"
#include "renderer/hal/interface/RenderContext.hpp"

namespace mce
{
	class BufferBase
	{
	public:
        BufferType m_bufferType;
        unsigned int m_stride;
        unsigned int m_count;
        unsigned int m_internalSize;
		
        BufferBase();
        ~BufferBase();
		BufferBase(const BufferBase& other);
		BufferBase(BufferBase&& other);

        void release();
        void bindBuffer(RenderContext& context) {}
		void createBuffer(RenderContext& context, unsigned int stride, const void *data, unsigned int count, BufferType bufferType);
		void createDynamicBuffer(RenderContext& context, unsigned int size, BufferType bufferType, const void *data);
        void resizeBuffer(RenderContext& context, const void* data, unsigned int size) {}
        void updateBuffer(RenderContext& context, unsigned int stride, const void *data, unsigned int count);
        unsigned int getInternalBufferSize() const { return m_internalSize; }
        bool isValid() const { return false; }

        BufferBase& operator=(BufferBase&& other);
	};
}
