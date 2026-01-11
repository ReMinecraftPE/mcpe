#include "ConstantBufferBase.hpp"

using namespace mce;

void ConstantBufferBase::createConstantBuffer(RenderContext &context, unsigned int count)
{
    m_bindingSlots.resize(count);
}

unsigned int& ConstantBufferBase::getInternalMemory()
{
    return m_bindingSlots.back();
}
