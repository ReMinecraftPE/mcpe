#pragma once

#include "renderer/hal/enums/BufferType.hpp"

namespace mce
{
	class RenderContext;
	
	class ImmediateBufferBase
	{
	public:
        BufferType m_bufferType;
		
        ImmediateBufferBase();

        void createDynamicBuffer(RenderContext& context, unsigned int bufferSize, BufferType bufferType, const void *data);
		void updateBuffer(RenderContext& context, unsigned int itemSize, void const* data, unsigned int bufferSize);

		bool isValid() const;
	};
}
