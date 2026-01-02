#pragma once
#include <stdint.h>
#include <cstring>
#include "renderer/hal/interface/ShaderConstant.hpp"
#include "renderer/MatrixStack.hpp"

namespace mce
{
    class ShaderConstantWithDataBase : public ShaderConstant
    {
    public:
        uint8_t* m_data;

    public:
        ShaderConstantWithDataBase(ShaderPrimitiveTypes primitiveType)
        {
            m_shaderPrimitiveType = primitiveType;
        }

        void setData(const void* data)
        {
            assert(data != nullptr);
            memcpy(m_data, data, getSize());
            m_dirty = true;
        }

#if MCE_GFX_ROW_MAJOR
        void setData(const Matrix* matrix)
        {
            assert(matrix != nullptr);
            Matrix tMatrix = *matrix;
            tMatrix.transpose();
            setData((const void*) &tMatrix);
        }
#endif
    };
}
