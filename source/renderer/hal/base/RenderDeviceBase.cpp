#include "RenderDeviceBase.hpp"

using namespace mce;

RenderDeviceBase::RenderDeviceBase()
{
    m_bDeviceLost = false;
}

const RenderDeviceBase::AttributeList& RenderDeviceBase::getAttributeList(unsigned int index) const
{
    return m_registeredAttributeLists[index];
}

unsigned int RenderDeviceBase::registerOrGetAttributeListIndex(const AttributeList& attributeList)
{
    for (int i = 0; i < m_registeredAttributeLists.size(); i++)
    {
        if (m_registeredAttributeLists[i] == attributeList)
        {
            return i;
        }
    }

    m_registeredAttributeLists.push_back(attributeList);

    return m_registeredAttributeLists.size() - 1;
}