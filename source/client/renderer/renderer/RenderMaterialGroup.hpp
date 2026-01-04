#pragma once

#include <map>
#include <set>
#include <string>

#include "thirdparty/rapidjson/document.h"

#include "client/app/AppPlatformListener.hpp"
#include "renderer/MaterialPtr.hpp"

namespace mce
{
    class RenderMaterialGroup : public AppPlatformListener
    {
    public:
        static RenderMaterialGroup common, switchable;

    public:
        std::map<const std::string, RenderMaterial> m_materials;
        std::set<MaterialPtr*> m_references;
        std::string m_listPath;

    public:
        RenderMaterialGroup();
        ~RenderMaterialGroup();

    protected:
        void _fireGroupReloaded();
        void _fireGroupDestroyed();
        RenderMaterial& _material(const std::string& fileName, const std::string& tag);
        void _loadMaterialSet(const rapidjson::Value& root, RenderMaterial& groupBaseParent, const std::string& materialIdentifier);
        void _loadList();

    // These are public despite being prefixed with underscores
    public:
        void _addRef(MaterialPtr& reference);
        void _removeRef(MaterialPtr& reference);
        RenderMaterial* _getMaterialPtr(const std::string& name);
        RenderMaterial* _getMaterial(const std::string& name);
        RenderMaterial& _getMaterialOrDefault(const std::string& name, RenderMaterial& defaultMaterial);

        MaterialPtr getMaterial(const std::string& name);
        void loadList(const std::string listPath);
        void compileMaterials();

        void onAppResumed() override;
        void onAppSuspended() override;
    };

#define GET_MATERIAL_PTR(group, name) mce::RenderMaterialGroup::group.getMaterial(name)
#define MATERIAL_PTR(group, name) name = GET_MATERIAL_PTR(group, #name)
}