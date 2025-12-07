#pragma once

#include "common/utility/Singleton.hpp"
#include "RenderChunkConstants.hpp"
#include "WorldConstants.hpp"
#include "PerFrameConstants.hpp"
#include "EntityConstants.hpp"
#include "ShaderConstants.hpp"
#include "WeatherConstants.hpp"

namespace mce
{
    class GlobalConstantBuffers : public Singleton<GlobalConstantBuffers>
    {
    public:
        RenderChunkConstants m_renderChunkConstants;
        WorldConstants m_worldConstants;
        PerFrameConstants m_perFrameConstants;
        EntityConstants m_entityConstants;
        ShaderConstants m_shaderConstants;
        WeatherConstants m_weatherConstants;

    public:
        void init();
    };
}