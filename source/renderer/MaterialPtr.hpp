#pragma once

#include <string>
#include "compat/LegacyCPP.hpp"

namespace mce
{
    class RenderMaterial;
    class RenderMaterialGroup;

    class MaterialPtr
    {
    public:
        static MaterialPtr NONE;

    public:
        RenderMaterialGroup* m_pGroup;
        RenderMaterial* m_pMaterial;
        std::string m_name;

    public:
        MaterialPtr();
        MC_CTOR_MOVE(MaterialPtr);
        MaterialPtr(RenderMaterialGroup& group, std::string const& name);
        ~MaterialPtr();

        void _deref();
        void _move(MaterialPtr& other);

        void onGroupReloaded();

        RenderMaterial* operator->() const;
        operator bool() const;
        MC_FUNC_MOVE(MaterialPtr);
        bool operator==(const MaterialPtr& other) const;
        bool operator!=(const MaterialPtr& other) const;
    };
}