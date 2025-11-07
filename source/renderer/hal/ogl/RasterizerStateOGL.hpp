#pragma once

#include "API_OGL.hpp"
#include "renderer/hal/base/RasterizerStateBase.hpp"
#include "renderer/hal/interface/RenderContext.hpp"

namespace mce
{
    class RasterizerStateOGL : public RasterizerStateBase
    {
    private:
        GLenum m_cullFace;
        bool m_enableScissorTest;
        bool m_cullMode;
        GLfloat m_depthBias;

    public:
        RasterizerStateOGL();

    public:
        bool bindRasterizerState(RenderContext& context, bool forceBind = false);
        void createRasterizerStateDescription(RenderContext& context, const RasterizerStateDescription& desc);
        void setScissorRect(RenderContext &context, int x, int y, int width, int height);
    };
}