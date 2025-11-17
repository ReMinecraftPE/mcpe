#pragma once

#include <string>

namespace mce
{
    class RenderMaterial;
    class RenderMaterialGroup;

    // @NOTE: Requires C++11 due to rvalues
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
        MaterialPtr(MaterialPtr&& other);
        MaterialPtr(RenderMaterialGroup& group, std::string const& name);
        ~MaterialPtr();

        void _deref();
        void _move(MaterialPtr&& other);

        void onGroupReloaded();

        RenderMaterial* operator->() const;
        operator bool() const;
        MaterialPtr& operator=(MaterialPtr&& other);
        bool operator==(const MaterialPtr& other) const;
        bool operator!=(const MaterialPtr& other) const;
    };
}