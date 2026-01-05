#include "ConstantBufferConstantsBase.hpp"
#include "renderer/RenderContextImmediate.hpp"

using namespace mce;

ConstantBufferConstantsBase::ConstantBufferConstantsBase()
{
    m_constantBuffer = nullptr;
}

void ConstantBufferConstantsBase::sync()
{
    m_constantBuffer->sync(RenderContextImmediate::get());
}