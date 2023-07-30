#include "DynamicTexture.hpp"
#include "Utils.hpp"

DynamicTexture::DynamicTexture(int a2) : m_textureIndex(a2)
{
	memset(m_pixels, 0, sizeof m_pixels);
}

void DynamicTexture::bindTexture(Textures* pTextures)
{
	pTextures->loadAndBindTexture(C_TERRAIN_NAME);
}

DynamicTexture::~DynamicTexture()
{

}
