#pragma once

#include <vector>

#include "renderer/Attribute.hpp"

namespace mce
{
    class RenderContext;

    class RenderDeviceBase
    {
    public:
        // Microsoft/Mojang were braindead and made this a struct with just `m_attributeList`
        typedef std::vector<Attribute> AttributeList;

        std::vector<AttributeList> m_registeredAttributeLists;
        bool m_bDeviceLost;

    public:
        RenderDeviceBase();

    public:
        const AttributeList& getAttributeList(unsigned int index) const;
        unsigned int registerOrGetAttributeListIndex(const AttributeList& attributeList);
    };
}