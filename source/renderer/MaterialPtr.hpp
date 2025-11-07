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
        RenderMaterialGroup* m_group;
        RenderMaterial* m_material;
        std::string m_name;

    public:
        MaterialPtr();
        MaterialPtr(MaterialPtr&& other);
        MaterialPtr(RenderMaterialGroup& group, std::string const& name);
        ~MaterialPtr();

        void _deref();
        void _move(MaterialPtr&& other);

        void onGroupReloaded();

        MaterialPtr& operator=(MaterialPtr&& other);
    };
}