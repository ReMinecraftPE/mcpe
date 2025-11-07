#pragma once

#include "renderer/hal/interface/ConstantBufferContainer.hpp"

namespace mce
{
    class ConstantBufferConstantsBase
    {
    protected:
        ConstantBufferContainer *m_constantBuffer;
            
    public:
        ConstantBufferConstantsBase();
        virtual ~ConstantBufferConstantsBase() {};
    
        virtual void init() = 0;
    };
}
