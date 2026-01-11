#include "GlobalConstantBuffers.hpp"

using namespace mce;

void GlobalConstantBuffers::init()
{
    m_worldConstants.init();
    m_renderChunkConstants.init();
    m_perFrameConstants.init();
    m_entityConstants.init();
    m_shaderConstants.init();
    m_weatherConstants.init();
}
