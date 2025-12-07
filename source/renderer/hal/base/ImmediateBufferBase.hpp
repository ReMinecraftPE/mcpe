#pragma once

#include "renderer/hal/enums/BufferType.hpp"

namespace mce
{
	class RenderContext;
	
	class ImmediateBufferBase
	{
	public:
        ImmediateBufferBase();

        void createDynamicBuffer(RenderContext& context, unsigned int stride, const void* data, unsigned int count, BufferType bufferType);
		void updateBuffer(RenderContext& context, unsigned int stride, void const* data, unsigned int count);

		bool isValid() const;
	};
}
