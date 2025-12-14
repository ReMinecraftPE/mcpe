#pragma once

#include "API_OGL.hpp"
#include "renderer/hal/base/DepthStencilStateBase.hpp"

namespace mce
{
    class DepthStencilStateOGL : public DepthStencilStateBase
    {
    private:
        struct StencilFaceDescriptionOGL
        {
            GLenum func;
            GLenum stencilFailAction;
            GLenum stencilPassDepthFailAction;
            GLenum stencilPassDepthPassAction;

            StencilFaceDescriptionOGL()
            {
                func = GL_NONE;
                stencilFailAction = GL_NONE;
                stencilPassDepthFailAction = GL_NONE;
                stencilPassDepthPassAction = GL_NONE;
            }
        };

    private:
        GLenum m_depthFunc;
        StencilFaceDescriptionOGL m_frontFaceStencilInfo;
        StencilFaceDescriptionOGL m_backFaceStencilInfo;
        GLuint m_stencilReadMask;
        GLuint m_stencilWriteMask;
        uint8_t m_depthWriteMask;

    public:
        DepthStencilStateOGL();

    public:
        void createDepthState(RenderContext& context, const DepthStencilStateDescription& description);
        bool bindDepthStencilState(RenderContext& context, bool force = false);
    };
}